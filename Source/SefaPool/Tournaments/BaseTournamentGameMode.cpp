// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseTournamentGameMode.h"

#include <random>


ABaseTournamentGameMode::ABaseTournamentGameMode() : AVarificationGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

#if UE_SERVER
	AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
#endif
	FNetworkVersion::IsNetworkCompatibleOverride.BindLambda([](uint32 LocalNetworkVersion, uint32 RemoteNetworkVersion)
	{
		return true;
	});
}

void ABaseTournamentGameMode::ReadyAgones(const FGameServerResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Readying game server"));
    FAgonesErrorDelegate error_del;
    error_del.BindUFunction(this, "AgonesError");
    FReadyDelegate success_del;
    success_del.BindUFunction(this, "AgonesSuccess");
    AgonesSDK->Ready(success_del, error_del);
}

void ABaseTournamentGameMode::AgonesSuccess(const FEmptyResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocation was succesful"));
    
    FGameServerDelegate del;
    del.BindUFunction(this, "GameServerCallback");
    AgonesSDK->WatchGameServer(del);
    
}

void ABaseTournamentGameMode::AgonesError(const FAgonesError& Response) {
    UE_LOG(LogTemp, Error, TEXT("%s"), *Response.ErrorMessage);
}

void ABaseTournamentGameMode::BeginPlay()
{
	Super::BeginPlay();
	SetVerification(false);
	TRACE("Tournament BeginPlay started");
    
    FGameServerResponse tmp = {};
    ReadyAgones(tmp);
    FScriptDelegate agones_reply_delegat;
    agones_reply_delegat.BindUFunction(this, "ReadyAgones");
    AgonesSDK->ConnectedDelegate.AddUnique(agones_reply_delegat);

	UPoolServerGameInstance* gi = Cast<UPoolServerGameInstance> (UGameplayStatics::GetGameInstance(GetWorld()));
	if (!gi) {
		UE_LOG(LogTemp, Error, TEXT("Game instance is nullptr"));
		return;
	}
	tournament = NewObject<UTournament>(this);
	//Not sure about static_cast. Using dynamic_cast gives a compile error because /GR-. Another solution might be to add bUseRTTI = true; to build.cs file.
	UBackendTournamentInfo* backend_data = dynamic_cast<UBackendTournamentInfo*>(gi->backend_data);
	if (!backend_data) {
		UE_LOG(LogTemp, Warning, TEXT("backend_data is nullptr, using default values."));
		tournament->Init(this, 1, EDifficultyLevels::DIFFICULTY_LEVEL_6, 4);
		return;
	}


	tournament->Init(this, backend_data->tourn_id, backend_data->difficulty, backend_data->size);
    
	UE_LOG(LogTemp, Warning, TEXT("Tournament opened with tourn_id: %d, dfficulty: %d, size: %d"), tournament->tourn_id, static_cast<int>(tournament->difficulty), tournament->size)
}

void ABaseTournamentGameMode::PreLogin(const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
    //TODO: pc_to_userid changes its size in Login, it's possible that between last player PreLogin and last player Login will be another connection
	if (tournament->userid_to_pc.Num() >= tournament->size)
		ErrorMessage = "Tournament is already full";
	
	if (tournament->has_started) {
		FString user_id = *UGameplayStatics::ParseOption(Options, "user_id");
		if (!tournament->userid_to_pc.Find(user_id)) {
			ErrorMessage = "This player don't participate in this tournament.";
		}
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

APlayerController* ABaseTournamentGameMode::Login(UPlayer* NewPlayer,
	ENetRole InRemoteRole,
	const FString& Portal,
	const FString& Options,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	APlayerController* pc = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
    
    AgonesSDK->Allocate({}, {});
	
	if (tournament->has_ended)
	{
		TRACE("Telling player that he is the winner");
		Cast<ATournamentPlayerController>(pc)->is_winner = true;
	}
	
	return pc;
}

void ABaseTournamentGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (Cast<ATournamentPlayerController>(NewPlayer)) {
		UE_LOG(LogTemp, Display, TEXT("PostLogin saved options: %s"), *Cast<ATournamentPlayerController>(NewPlayer)->Options);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Can't cast PC to TOURNAMENT PC"));
	}

	if (!tournament->has_ended)
	{
		tournament->RegisterIncomingPlayer(NewPlayer);
		UpdateListOfPlayers();
	}
	else
	{
		FTimerHandle timer;
		GetWorld()->GetTimerManager().SetTimer(timer, this, &ABaseTournamentGameMode::ShutdownServer, 90, false);
	}
}

void ABaseTournamentGameMode::ShutdownServer()
{
#if UE_SERVER
	UE_LOG(LogTemp, Error, TEXT("Shutting down tournament lobby"));
	AgonesSDK->Shutdown({}, {});
	FGenericPlatformMisc::RequestExit(false);
#endif
}

void ABaseTournamentGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}

void ABaseTournamentGameMode::UpdateListOfPlayers()
{
	TArray<FString> user_ids;
	tournament->userid_to_pc.GetKeys(user_ids);
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		auto pc = Cast<ATournamentPlayerController>(*Iterator);
		if (IsValid(pc))
		{
			pc->UpdateListOfPlayers(user_ids);
		}
	}
}

