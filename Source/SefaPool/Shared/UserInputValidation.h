// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SefaPoolGameInstance.h"
#include "PoolPlayerControllerBase.h"
#include "UserInputValidation.generated.h"

class AObjectGatherer;

UCLASS()
class SEFAPOOL_API AUserInputValidation : public AActor
{
	GENERATED_BODY()
    AObjectGatherer* objects;
public:	
	// Sets default values for this actor's properties
    AUserInputValidation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
    void ValidateShot(APoolPlayerControllerBase* pc,
                      FVector target,
                      FVector2D offset,
                      float power,
                      bool usingAngle);
	
    void ValidateCueChange(APoolPlayerControllerBase* pc, int num);
    void ValidateChalkChange(APoolPlayerControllerBase* pc, int num);
    void ValidateChalkApplication(APoolPlayerControllerBase* pc);
    void ValidateBallInHandNewCueBallPos(APoolPlayerControllerBase* pc, FVector pos);
    void ValidateConsumableRecharge(APoolPlayerControllerBase* pc, EConsumableType type, int num);
	
};
