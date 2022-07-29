// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallsGathererBase.generated.h"

UCLASS()
class SEFAPOOL_API ABallsGathererBase : public AActor
{
	GENERATED_BODY()

public:
	ABallsGathererBase();

protected:
	uint8 BallsCountInGame;
	
	UPROPERTY(EditDefaultsOnly);
	TSoftObjectPtr<class UmaterialInterface> BallsMaterials;
	
	void BeginPlay() override;

public:
	void SetBallsMaterials();


};
