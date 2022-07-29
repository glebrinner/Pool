// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SefaPool/PlayersVarification/VarificationPlayerState.h"
#include "../Widgets/PoolTournamentWidget.h"
#include "../Shared/SharedDefs.h"
#include "Kismet/GameplayStatics.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPool/Widgets/LoadingWidget.h"

#include "TournamentPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API ATournamentPlayerState : public AVarificationPlayerState
{
	GENERATED_BODY()
public:
	void BeginPlay() override;

public:
	UFUNCTION(Client, reliable)
	void TravelToPoolGame(const FName& level_name, const FString& server_user_id);
};
