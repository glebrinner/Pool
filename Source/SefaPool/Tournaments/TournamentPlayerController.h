// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SefaPool/PlayersVarification/VarificationPlayerController.h"
#include "../Widgets/PoolTournamentWidget.h"
#include "TimerManager.h"
#include "../Shared/SharedDefs.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

#include "TournamentPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API ATournamentPlayerController : public AVarificationPlayerController
{
	GENERATED_BODY()
	
public:
	//ATournamentPlayerController();

	void BeginPlay() override;

	UFUNCTION(Client, reliable)
	void UpdateListOfPlayers(const TArray<FString>& players);

	UFUNCTION(Client, reliable)
	void ShowWinScreen();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

public:
	UPROPERTY()
	UPoolTournamentWidget* tournament_widget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
	TSubclassOf<UPoolTournamentWidget> tournament_widget_class;

	UPROPERTY(replicated)
	bool is_winner;


private:
	TArray<FString> current_players;
};

