// Fill out your copyright notice in the Description page of Project Settings.


#include "TournamentPlayerController.h"

//ATournamentPlayerController::ATournamentPlayerController()
//{
//
//}

void ATournamentPlayerController::BeginPlay()
{
	Super::BeginPlay();
	PRINTF((TEXT("Begin play started.")), Red);
	if (IsLocalPlayerController() && tournament_widget_class)
	{
		tournament_widget = CreateWidget<UPoolTournamentWidget>(this, tournament_widget_class);
		if (!tournament_widget)
		{
			PRINTF((TEXT("Problem with creating widget")), Red);
		}
		else
		{
			PRINTF((TEXT("is_winner: %d"), is_winner), Red);
			if (is_winner)
			{
				ShowWinScreen();
			}
			tournament_widget->AddToViewport();
		}
        
        auto* gi = Cast<USefaPoolGameInstance>(GetGameInstance());
        if (gi)
        {
            gi->orientationManager->SwitchToOrientation(E_ScreenOrientation::ORIENTATION_PORTRAIT_LOCKED, this);
        }
	}
	UpdateListOfPlayers(current_players);
    
    
}

void ATournamentPlayerController::UpdateListOfPlayers_Implementation(const TArray<FString>& players)
{
	current_players = players;
	//if (!HasActorBegunPlay())
	//{
	//	PRINTF((TEXT("BeginPlay wasn't started yet")), Red);
	//}
	if (tournament_widget)
	{
		tournament_widget->UpdatePlayersList(players);
	}
}

void ATournamentPlayerController::ShowWinScreen_Implementation()
{
	USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
	FString username = "";
	if (gi)
	{
		PRINTF((TEXT("GI in TournamentPlayerController was valid")), Yellow);
		username = gi->data->user->username;
	}
	else 
	{
		PRINTF((TEXT("GI in TournamentPlayerController wasn't valid")), Yellow);
	}
	if (tournament_widget)
	{
		PRINTF((TEXT("Calling EndTournament on widget")), Yellow);
		tournament_widget->EndTournament(username, 0);
	}
	else
	{
		PRINTF((TEXT("Tournament widget wasn't valid")), Yellow);
	}
}

void ATournamentPlayerController::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATournamentPlayerController, is_winner);
}
