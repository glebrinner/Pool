// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SefaPool/Shared/PoolPlayerControllerBase.h"
#include "GameFramework/PlayerController.h"
#include "ReplayPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API AReplayPlayerController : public APoolPlayerControllerBase
{
	GENERATED_BODY()
	class AObjectGatherer* ObjectGatherer;
public:
	
	AReplayPlayerController();

	UFUNCTION(BlueprintCallable)
	void RestartRecording();

protected:
	void BeginPlay() override;
	
};
