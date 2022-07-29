// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "../Shared/SharedDefs.h"
#include "Kismet/KismetStringLibrary.h"
#include "LobbyGameMode.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"


void ALobbyPlayerState::BeginPlay()
{
	Super::BeginPlay();
	//PrimaryActorTick.bCanEverTick = false;
	FString auth;
	if (HasAuthority()) {
		auth = "Server Player state begin play";
	}
	else {
		auth = "Client Player state begin play";
	}
	UE_LOG(LogTemp, Warning, TEXT("%s"), *auth);
	PRINTF((TEXT("%s"), *auth), White);
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(WaitingTimer, this,
			&ALobbyPlayerState::UpdateTime, 1, true);
	}
}

//void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps); 
//
//	DOREPLIFETIME(ALobbyPlayerState, stake_);
//	DOREPLIFETIME(ALobbyPlayerState, difficulty_);
//}

void ALobbyPlayerState::ClientRPCFunction_Implementation(int stake_, int difficulty_, int user_id_) {
	stake = stake_;
	difficulty = difficulty_;
	user_id = user_id_;

}

void ALobbyPlayerState::UpdateTime()
{

	if (GetGameTimeSinceCreation() - waiting_time_begin > MAX_WAITING_TIME)
	{
		waiting_time_begin = GetGameTimeSinceCreation();
	}
}

void ALobbyPlayerState::TravelToPoolGame_Implementation(const FString& ip, int32 port) {
	FString Options = FString("user_id=") + FString::FromInt(user_id) + "?";
    
	FName level_name = FName(ip + ":" + FString::FromInt(port));
	UE_LOG(LogTemp, Warning, TEXT("traveling to %s"), *(level_name.ToString()));
	
	USefaPoolGameInstance* gi = Cast<USefaPoolGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (gi)
	{
		ULoadingWidget* wg = Cast<ULoadingWidget>(gi->widgetManager->GetWidget("Loading"));
		if (wg)
			wg->ChangeVisibleText("Creating pool game ...");
		else
			PRINTF((TEXT("Widget was nullptr")), Red);
	}
	else
	{
		PRINTF((TEXT("GI was nullptr")), Red);
	}
	UGameplayStatics::OpenLevel(GetWorld(), level_name, true, Options);
}

void ALobbyPlayerState::WritePlayerData(FString options)
{
	stake = FCString::Atoi(*UGameplayStatics::ParseOption(options, "stake"));
	difficulty = FCString::Atoi(*UGameplayStatics::ParseOption(options, "difficulty"));
	user_id = FCString::Atoi(*UGameplayStatics::ParseOption(options, "player_id"));
	access_token = UGameplayStatics::ParseOption(options, "access_token");
}
