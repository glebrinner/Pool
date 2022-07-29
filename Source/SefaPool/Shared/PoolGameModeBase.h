// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "Dom/JsonObject.h"
#include "PoolPlayerStateBase.h"
#include "PoolGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APoolGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	protected:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class ARefereeBase> Referee_Class;

	void BeginPlay() override;
public:
	ARefereeBase* Referee;
	
    virtual void PCReady(class APoolPlayerControllerBase* player);
    virtual void ProcessBallPocketed(int num, bool first) ;
    virtual void ProcessBallKnockedOff(int num);
    virtual void ProcessHitLogAndFinalize(const TArray<int>& hitLog1,
                                          const TArray<int>& hitLog2);
    virtual bool CanPlayerShoot (APoolPlayerControllerBase* player);
    virtual void OnPlayerShotEvent(uint8 PlayerID = 0);              
    
    virtual void StartingPhysicsSimulationForShot (APoolPlayerControllerBase* player);

    virtual TSharedPtr<FJsonObject> CreateConsolidateJson(APoolPlayerStateBase* ps);
    
    UFUNCTION(BlueprintCallable)
    virtual void LevelReady();
};
