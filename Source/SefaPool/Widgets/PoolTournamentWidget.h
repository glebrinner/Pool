// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PoolTournamentWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolTournamentWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent, Category = "UI Updating")
	void UpdatePlayersList(const TArray<FString> &players);

	UFUNCTION(BlueprintImplementableEvent, Category = "UI Updating")
	void EndTournament(const FString& nickname, int amount);
};
