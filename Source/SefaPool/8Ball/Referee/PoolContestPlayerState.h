// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Pool8BallPlayerState.h"
#include "PoolContestPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APoolContestPlayerState : public APoolPlayerStateBase
{
	GENERATED_BODY()
public:
    UPROPERTY(replicated, BlueprintReadWrite)
    int32 pocketedBallsCount;
    
    UPROPERTY(replicated, BlueprintReadWrite)
    float inGameTime;
    
    virtual bool CanTargetThisBall(int n);
    
    
    virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
};
