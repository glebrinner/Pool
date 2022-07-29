// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolSPGameMode.h"
#include "../Shared/PoolGameStateBase.h"
#include "../Shared/PoolPlayer.h"
#include "../Shared/PoolHUD.h"
#include "../Shared/SharedDefs.h"
#include "../Shared/PoolPlayerStateBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "SefaPool/8Ball/Referee/SPReferee.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "AgonesComponent.h"
#include "../Server/PoolServerGameInstance.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"


APoolSPGameMode::APoolSPGameMode() :
#if UE_SERVER
    AgonesSDK           (nullptr),
#endif
	playingPc           (),
	levelReady          (false),
    firstShot           (true)
{
	GameStateClass = APoolGameStateBase::StaticClass();
	PlayerControllerClass = APoolSPPlayerController::StaticClass();
    PlayerStateClass = APoolPlayerStateBase::StaticClass();
    DefaultPawnClass = APoolPlayer::StaticClass();
	HUDClass = APoolHUD::StaticClass();
#if UE_SERVER
    AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
    FNetworkVersion::IsNetworkCompatibleOverride.BindLambda([](uint32 LocalNetworkVersion, uint32 RemoteNetworkVersion)
    {
        return true;
    });
#endif
}


void APoolSPGameMode::PreLogin(const FString& Options,
                               const FString& Address,
                               const FUniqueNetIdRepl& UniqueId,
                               FString& ErrorMessage)
{
#if UE_SERVER
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi)
    {
        ErrorMessage = TEXT("Game instance not found");
    }
    FString user_id = UGameplayStatics::ParseOption(Options, "user_id");
    if (gi && !gi->backend_data->GetPlayerInfo(user_id))
    {
        ErrorMessage = TEXT("Incorrect user_id");
    }
#endif
    Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* APoolSPGameMode::Login(UPlayer* NewPlayer,
    ENetRole InRemoteRole,
    const FString& Portal,
    const FString& Options,
    const FUniqueNetIdRepl& UniqueId,
    FString& ErrorMessage)
{
    UE_LOG(LogTemp, Warning, TEXT("Login"));
    APlayerController* pc = Super::Login(NewPlayer, InRemoteRole, Portal, Options,  UniqueId, ErrorMessage);
    Cast<APoolPlayerControllerBase>(pc)->Options_ = Options;
    return pc;
}

void APoolSPGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);
    APoolPlayerControllerBase* pc = Cast<APoolPlayerControllerBase>(NewPlayer);
    APoolPlayerStateBase* ps = Cast<APoolPlayerStateBase>(pc->PlayerState);
    #if UE_SERVER
        ps->user_id = (UGameplayStatics::ParseOption(pc->Options_, "user_id"));
    #endif
}


void APoolSPGameMode::Logout(AController* Exiting)
{
    UE_LOG(LogTemp, Warning, TEXT("Player Logout"));
    auto pc_exiting = Cast<APoolPlayerControllerBase>(Exiting);
#if UE_SERVER
    FTimerHandle timer;
    GetWorld()->GetTimerManager().SetTimer(timer, this, &APoolSPGameMode::ExitServer, 5, false);
#endif
    Super::Logout(Exiting);
}

void APoolSPGameMode::ExitServer() {
#if UE_SERVER
    AgonesSDK->Shutdown({}, {});
    FGenericPlatformMisc::RequestExit(false);
#endif
}

void APoolSPGameMode::StartGame()
{
	Referee->NextShot(1);
	objects->ballinfo->UpdateBallPosOnClients();
	
}

void APoolSPGameMode::BeginPlay()
{
	Super::BeginPlay();
    
#if UE_SERVER
    AgonesSDK->Allocate({}, {});
    FScriptDelegate agones_reply_delegat;
    agones_reply_delegat.BindUFunction(this, "AllocateAgones");
    AgonesSDK->ConnectedDelegate.AddUnique(agones_reply_delegat);
#endif
    
	GATHER_OBJECT_GATHERER(objects);
}

void APoolSPGameMode::AllocateAgones(const FGameServerResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocating game server"));
#if UE_SERVER
    FAgonesErrorDelegate error_del;
    error_del.BindUFunction(this, "AgonesError");
    FAllocateDelegate success_del;
    success_del.BindUFunction(this, "AgonesSuccess");
    AgonesSDK->Allocate(success_del, error_del);
#endif
}

void APoolSPGameMode::AgonesSuccess(const FEmptyResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocation was succesful"));
}

void APoolSPGameMode::AgonesError(const FAgonesError& Response) {
    UE_LOG(LogTemp, Error, TEXT("%s"), *Response.ErrorMessage);
}

void APoolSPGameMode::PCReady(APoolPlayerControllerBase* pc)
{
	if (!this || !pc)
	{ERROR_DEBUG_PRINT_FUNC}
	Referee->PlayingPc =  pc;
    APoolSPPlayerController* player = dynamic_cast<APoolSPPlayerController*> (pc);
    if (!player)
        ERROR_DEBUG_PRINT_FUNC
	player->ClientSetBallManagerSettings(Referee->FirstBallIndex,Referee->BallCountInGame);
	player->ClientSetViewToTable();
    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GameState);
    if (!gs)
        ERROR_DEBUG_PRINT_FUNC
	Referee->ShuffleAndArrangeBalls();
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
    UPoolServerGameInstance* srv_gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (ps && !srv_gi)  // Client SP
    {
		auto GameInstance = Cast<USefaPoolGameInstance>(pc->GetGameInstance());
		TArray<int32> cue_durabilities, chalk_durabilities;
		int current_cue = 1, current_chalk = 1;
		if (GameInstance && GameInstance->data->inventory->cue_durabilities.Num() != 0)
		{
			cue_durabilities = GameInstance->data->inventory->cue_durabilities;
			current_cue = GameInstance->data->inventory->current_cue;
            PRINTF((TEXT("Entry data found")), White)
		}
		else // PIE
		{
			cue_durabilities.Init(200, PREDEFINED_CUES_N + 1);
            cue_durabilities[0] = -1;
			current_cue = 20;
            PRINTF((TEXT("Entry data not found")), White)
		}
        
		if (GameInstance && GameInstance->data->inventory->chalk_durabilities.Num() != 0)
		{
			chalk_durabilities = GameInstance->data->inventory->chalk_durabilities;
			current_chalk = GameInstance->data->inventory->current_chalk;
		}
		else // PIE
		{
			chalk_durabilities.Init(-1, PREDEFINED_CHALKS_N + 1);
            chalk_durabilities[1] = 200; chalk_durabilities[2] = 200; chalk_durabilities[3] = 200; chalk_durabilities[4] = 200;
			current_chalk = 2;
		}
        ps->username = GameInstance->data->user->username;
        ps->InitClient(cue_durabilities,
                       chalk_durabilities,
                       current_cue,
                       current_chalk,
                       GameInstance->DiffLevel);
    }
    else
    if (ps && srv_gi) // Server-based SP (contests)
    {
        UBaseBackendInfo* info = dynamic_cast<UBaseBackendInfo*> (srv_gi->backend_data);
        if (info)
        {
            FBackendPlayerInfo* pi_ = info->GetPlayerInfo(ps->user_id);
            if (pi_)
            {
                TArray<int32> cue_durabilities, chalk_durabilities;

                cue_durabilities = pi_->durability.cue_durabilities;
                chalk_durabilities = pi_->durability.chalk_durabilities;
                
                ps->username = pi_->username;
                EDifficultyLevels diff = EDifficultyLevels::DIFFICULTY_LEVEL_NONE;
                
                if (dynamic_cast<UBackendContestInfo*> (info))
                    diff = (dynamic_cast<UBackendContestInfo*> (info))->division;
                if (diff == EDifficultyLevels::DIFFICULTY_LEVEL_NONE)
                    diff = EDifficultyLevels::DIFFICULTY_LEVEL_6;
                ps->InitClient(cue_durabilities,
                    chalk_durabilities,
                    pi_->cur_cue,
                    pi_->cur_chalk,
                    diff);
            }
        }
        
    }
    
	playingPc = player;
    
    gs->objects->shotinfo->SetNetworked(GetNetMode() == NM_DedicatedServer);
    Referee->OnGameStart();
	if (levelReady)
	{
        PRINTF((TEXT("PCReady:: next shot")), Red)
		NextShot(playingPc);
	}
    else
    {
        PRINTF((TEXT("PCReady:: no next shot")), Red)
    }
	objects->ReplayManager->FirstPlayerNickName = ps->username;
}

void APoolSPGameMode::LevelReady()
{
	levelReady = true;
	PRINTF((TEXT("LevelReady")), Green)
	if (playingPc)
	{
		PRINTF((TEXT("LevelReady:: next shot")), Green)
		NextShot(playingPc);
	}
	else
		PRINTF((TEXT("no next shot")), Red)
        
    
}
void APoolSPGameMode::NextShot(APoolSPPlayerController* NewPlayer)
{
    // APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GameState);
    // if (gs)
    // {
    //     if (firstShot)
    //     {
    //         gs->SetRuleset(RULES_BEHIND_HEADSTRING);
    //         firstShot = false;
    //     }
    //     else
    //         gs->SetRuleset(RULES_ANYWHERE);
    // }
	Referee->NextShot(1);
	// Referee->NextShot(pc_num);
	objects->ballinfo->UpdateBallPosOnClients();
	NewPlayer->ClientStartPlay();
}

void APoolSPGameMode::ProcessBallPocketed(int num, bool first)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GameState);
    PRINTF((TEXT("Ball %d has been pocketed"), num), Blue)
	if (num == 0)
	{
        PRINTF((TEXT("Returning cue ball")), Blue)
		gs->ReturnCueBall();
	}
	else
	{
		// already taken care of
		//gs->StorePocketedBall(num);
	}
}
void APoolSPGameMode::ProcessBallKnockedOff(int num)
{
	// cue => issue foul/loss
	// other => return to point
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GameState);
	if (num == 0)
	{
		gs->ReturnCueBall();
	}
	else
	{
		gs->ReturnFoulBall(num);
	}
}
void APoolSPGameMode::ProcessHitLogAndFinalize(const TArray<int>& hitLog1, const TArray<int>& hitLog2)
{
	//PRINTF((TEXT(__FUNCSIG__)), Yellow)
	bool inGame = false;
	for (int i = 1; i < 16; i++)
	{
		if (dynamic_cast<APoolGameStateBase*> (GameState)->ballsInGame[i])
		{
			inGame = true;
			break;
		}
	}
	if (!inGame)
	{
        playingPc->ClientTravelToLevel("Authentication", "WinScreen");
	}
	else NextShot(playingPc);
	
}
