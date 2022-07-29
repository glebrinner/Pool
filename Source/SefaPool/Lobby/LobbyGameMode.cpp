// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
// Fill out your copyright notice in the Description page of Project Settings.
#include "LobbyPlayerController.h"
#include "LobbyPlayerState.h"
#include "../Shared/SharedDefs.h"
#include "HAL/RunnableThread.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/DateTime.h"
#include "Math/UnrealMathUtility.h"
#include "../Server/PoolServerGameInstance.h"
#include "GenericPlatform/GenericPlatformMisc.h"


ALobbyGameMode::ALobbyGameMode() :
	APlayerVarificationGameMode(),
	players_queues(),
	DifficultyToPlayers()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerControllerClass = ALobbyPlayerController::StaticClass();
	PlayerStateClass = ALobbyPlayerState::StaticClass();
#if UE_SERVER
	AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
#endif
	FNetworkVersion::IsNetworkCompatibleOverride.BindLambda([](uint32 LocalNetworkVersion, uint32 RemoteNetworkVersion)
	{
		return true;
	});
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
#if UE_SERVER
    AgonesSDK->Allocate({}, {});
    FScriptDelegate agones_reply_delegat;
    agones_reply_delegat.BindUFunction(this, "AllocateAgones");
    AgonesSDK->ConnectedDelegate.AddUnique(agones_reply_delegat);
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!srv_gi) return;
    gs_allocate = NewObject<UGsAsyncQueueAllocator> ();
    gs_allocate->Init(srv_gi);
#endif
}


void ALobbyGameMode::AllocateAgones(const FGameServerResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocating game server"));
#if UE_SERVER
    FAgonesErrorDelegate error_del;
    error_del.BindUFunction(this, "AgonesError");
    FAllocateDelegate success_del;
    success_del.BindUFunction(this, "AgonesSuccess");
    AgonesSDK->Allocate(success_del, error_del);
#endif
}

void ALobbyGameMode::AgonesSuccess(const FEmptyResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocation was succesful"));
}

void ALobbyGameMode::AgonesError(const FAgonesError& Response) {
    UE_LOG(LogTemp, Error, TEXT("%s"), *Response.ErrorMessage);
}

void ALobbyGameMode::PreLogin(const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
#if UE_SERVER
	if (GetNumPlayers() == 0)
	{
		AgonesSDK->Allocate({}, {});
	}
#endif
}

APlayerController* ALobbyGameMode::Login(UPlayer* NewPlayer,
	ENetRole InRemoteRole,
	const FString& Portal,
	const FString& Options,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	APlayerController* pc = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	Cast<ALobbyPlayerController>(pc)->options = Options;
	return pc;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ALobbyPlayerController* pc = Cast<ALobbyPlayerController>(NewPlayer);
	ALobbyPlayerState* ps = Cast<ALobbyPlayerState>(pc->PlayerState);
	ps->WritePlayerData(pc->options);
	ps->ClientRPCFunction(ps->stake, ps->difficulty, ps->user_id);
	CheckEnoughBalance(ps);
	UE_LOG(LogTemp, Warning, TEXT("Player state pointer in post login: %x"), ps);
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}


void ALobbyGameMode::CheckEnoughBalance(ALobbyPlayerState* ps)
{
    
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    
    if (srv_gi)
    {
        cool_json_t json;
        json["user_id"] = ps->user_id;
        auto callback = [=](cool_json_t json) -> void {
            if (!json.contains("status") || json["status"] != 200)
            {
                UE_LOG(LogTemp, Warning, TEXT("Bad status on core/get_balance."));
            }
            else
            {
                int balance = json["balance"].get<int>();
                if (balance >= ps->stake)
                {
                    AddPlayer(ps);
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("Not enough balance. Balance is: %d"), balance);
                    //Logout(dynamic_cast<AController*>(ps->GetOwner()));
                }
            }
        };
        
        srv_gi->MakeRequest("core", "get_balance", json, callback);
    }
}

void ALobbyGameMode::ConnectPlayersToGame(ALobbyPlayerState* ps1, ALobbyPlayerState* ps2, FString amount)
{
	if (!(IsValid(ps1) && IsValid(ps2)))
	{
		UE_LOG(LogTemp, Warning, TEXT("Traveling failed. Players was not valid"));
	}
	
	cool_json_t json = {
		{"user1_id", ps1->user_id},
		{"user2_id", ps2->user_id},
		{"type", static_cast<int>(EPoolGameType::GAME_1v1)},
		{"params", {
			{"amount", amount},
			{"difficulty", ps1->difficulty}
		}}
	};

    FAllocationRequest req = {};
    req.service = TEXT("core");
    req.endpoint = TEXT("internal/begin_type_1vs1");
    req.json = json;
    
    req.result_callback = [ps1, ps2](FString ip, int port, int match_id) {
        if (IsValid(ps1) && IsValid(ps2))
        {
            if (GS_IP_FOR_CLIENTS != std::string("NO")) ip = GS_IP_FOR_CLIENTS;
            
            UE_LOG(LogTemp, Warning, TEXT("Traveling to %s:%d"), *ip, port);
            ps1->TravelToPoolGame(ip, port);
            ps2->TravelToPoolGame(ip, port);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ALobbyGameMode::ConnectPlayersToGame::callback ps invalid"));
        }
	};

    gs_allocate->RequestAllocation(std::move(req));
}

void ALobbyGameMode::AddPlayer(ALobbyPlayerState* ps)
{
	if (IsValid(ps))
	{
		players_queues[{ps->difficulty, ps->stake}].push(ps);
		DifficultyToPlayers[ps->difficulty].insert(ps);
		UE_LOG(LogTemp, Warning, TEXT("Add player at difficulty: %d, stake: %d"), ps->difficulty, ps->stake);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't add player to Lobby, PlayerState wasn't valid"), ps->difficulty, ps->stake);
	}
}

ALobbyPlayerState* GetValidPlayer(std::queue<ALobbyPlayerState*>& players_queue) {
	while (!players_queue.empty())
	{
		ALobbyPlayerState* valid_ps = nullptr;
		valid_ps = players_queue.front();
		players_queue.pop();
		if (IsValid(valid_ps))
		{
			//PRINTLOG(TEXT("Find valid player"));
			return valid_ps;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Disconected player was deleted from queue."));
		}
	}
	return nullptr;
}

void ALobbyGameMode::UpdatePlayersQueues()
{
	for (auto& pair : players_queues)
	{
		auto& players_queue = pair.second;
		ALobbyPlayerState* ps1 = GetValidPlayer(players_queue);
		ALobbyPlayerState* ps2 = GetValidPlayer(players_queue);

		while (ps1 != nullptr && ps2 != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Two players were added at difficulty %d and stake %d"), pair.first.first, pair.first.second);
			ConnectPlayersToGame(ps1, ps2, FString::FromInt(pair.first.second));
			ps1 = GetValidPlayer(players_queue);
			ps2 = GetValidPlayer(players_queue);
		}

		if (ps1 != nullptr)
		{
			players_queue.push(ps1);
		}
	}
	
}

void ALobbyGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdatePlayersQueues();
}

void ALobbyGameMode::ConnectWithAnotherStake(ALobbyPlayerState* ps)
{
	auto& players = DifficultyToPlayers[ps->difficulty];
	auto it = players.find(ps);

}
