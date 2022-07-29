// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "../Shared/CueComponent.h"
#include "../Shared/SefaPoolGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "../Shared/SharedDefs.h"
#include "../Shared/PoolPlayerStateBase.h"
#include "Pool8BallPlayerState.generated.h"

UENUM(BlueprintType)
enum E_Pool8BallNextTarget
{ 
	LOW_BALLS = 0 UMETA(DisplayName = "Low balls"),
    HIGH_BALLS UMETA(DisplayName = "High balls"),
    OPEN_TABLE UMETA(DisplayName = "Open table"),
    BLACK_BALL UMETA(DisplayName = "Black ball"),
};

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APool8BallPlayerState : public APoolPlayerStateBase
{
	GENERATED_BODY()
public:
    UPROPERTY(replicated)
	TEnumAsByte<E_Pool8BallNextTarget> nextTarget;
    UPROPERTY(replicated)
	TArray<int> currentFrameNextBallsToHit;
    UPROPERTY(replicated)
	TArray<int> currentFramePocketed;
    
    int shots = 0;
    
public:
	APool8BallPlayerState();
	void UpdatePocketed();
	void ChangeNextTarget(E_Pool8BallNextTarget target);
    
    virtual void UpdateClient();
    
	/*UFUNCTION(Server, Reliable)
	void ServerForceReplicate();
	void ServerForceReplicate_Implementation();

	UFUNCTION(Client, Reliable)
	void ClientForceReplicate(E_Pool8BallNextTarget nextTarget_, bool ballInHand_, const TArray<int>& currentFrameNextBallsToHit_, const TArray<int>& currentFramePocketed_);
	void ClientForceReplicate_Implementation(E_Pool8BallNextTarget nextTarget_, bool ballInHand_, const TArray<int>& currentFrameNextBallsToHit_, const TArray<int>& currentFramePocketed_);*/

	UFUNCTION(BlueprintPure)
	E_Pool8BallNextTarget GetNextTarget() const;

	UFUNCTION(BlueprintPure)
	bool IsBallInHand()const;

	UFUNCTION(BlueprintPure)
    void CurrentFrameNextBallsToHit (TArray<int32>& arr)const;

	UFUNCTION(BlueprintPure)
	void CurrentFramePocketed (TArray<int32>& arr) const;
    
    virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
    
    virtual bool CanTargetThisBall(int n);
};
