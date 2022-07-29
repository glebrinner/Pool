// Fill out your copyright notice in the Description page of Project Settings.


#include "TournamentPlayerState.h"


void ATournamentPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ATournamentPlayerState::TravelToPoolGame_Implementation(const FName& level_name, const FString& server_user_id) {

	UE_LOG(LogTemp, Warning, TEXT("traveling to %s"), *(level_name.ToString()));
	FString Options = FString("user_id=") + server_user_id + "?";
	USefaPoolGameInstance* gi = Cast<USefaPoolGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	ULoadingWidget* wg = Cast<ULoadingWidget>(gi->widgetManager->GetWidget("Loading"));
	if (wg)
		wg->ChangeVisibleText("Connecting to game");
	else
		PRINTF((TEXT("Widget was nullptr")), Red);
	gi->OpenLevel(level_name.ToString(), Options);
	//UGameplayStatics::OpenLevel(GetWorld(), level_name, true, Options);
}
