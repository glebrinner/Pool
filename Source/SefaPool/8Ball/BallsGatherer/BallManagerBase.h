// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SefaPool/Shared/SharedDefs.h"
#include "GameFramework/Actor.h"
#include "BallManagerBase.generated.h"

UCLASS()
class SEFAPOOL_API ABallManagerBase : public AActor
{
	GENERATED_BODY()
	class AObjectGatherer* ObjectGatherer;
public:
	ABallManagerBase();

protected:

	uint8 BallsCountInScene = BALL_COUNT_IN_SCENE;
	
	// UPROPERTY(EditDefaultsOnly);
	// TSoftObjectPtr<class UmaterialInterface> BallsMaterials;
	
	void BeginPlay() override;

	bool bIsBallsSeted;

public:
	void SetBallsForGM();
	
	UPROPERTY(EditDefaultsOnly);
	uint8 BallsCountInGame;

	UPROPERTY(EditDefaultsOnly)
	uint8 FirstBallIndex;

};
