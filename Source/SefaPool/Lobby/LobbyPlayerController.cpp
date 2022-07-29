// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "../Shared/SharedDefs.h"

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		PRINTF((TEXT("Start check last ping timer")), Red);
		UE_LOG(LogTemp, Warning, TEXT("Start check last ping timer"));
		GetWorldTimerManager().SetTimer(CheckLastPingTimer, this,
			&ALobbyPlayerController::CheckLastPing, 1, true);
	}
}

//void ALobbyPlayerController::UpdateTime()
//{
//
//	if (GetGameTimeSinceCreation() - waiting_time_begin > MAX_WAITING_TIME)
//	{
//		waiting_time_begin = GetGameTimeSinceCreation();
//		UE_LOG(LogTemp, Warning, TEXT("Do smth"));
//	}
//}
//

//void ALobbyPlayerController::BeginPlay() {
//	Super::BeginPlay();
//	//FString s;
//	//if (HasAuthority()) {
//	//	s = "Server side PC Begin play";
//	//}
//	//else
//	//{
//	//	s = "Client side PC Begin play";
//	//}
//	//UE_LOG(LogTemp, Warning, TEXT("%s"), *s);
//	//PRINTF((TEXT("%s"), *s), White);
//	//bReplicates = true;
//}

//void TravelToPoolGame_Implementation(std::string level_name) {
//	UGameplayStatics::OpenLevel(GetWorld(), "level_name");
//}

void ALobbyPlayerController::Ping_Implementation()
{
	previous_ping = GetGameTimeSinceCreation();
}

void ALobbyPlayerController::CheckLastPing_Implementation()
{
	if (!IsClientAlive())
	{
		CloseConnection();
	}
}


// Return true if last ping delay from client was under limit.
bool ALobbyPlayerController::IsPingDelayCorrect()
{
	return is_ping_delay_correct;
}
