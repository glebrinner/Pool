// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolBaseWidget.h"
#include "PoolUserBarWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolUserBarWidgetBase : public UPoolBaseWidget
{
	GENERATED_BODY()
	
    
public:
    UFUNCTION(BlueprintCallable)
    void ShowShop();
    
	UFUNCTION(BlueprintNativeEvent)
	void UpdateUserTargetBalls(const TArray<int32>& nextBallsToHit);
    
    UFUNCTION(BlueprintNativeEvent)
    void UpdateUsername(const FString& username, bool opponent);
    
    UFUNCTION(BlueprintNativeEvent)
    void StartingAiming();
    
    UFUNCTION(BlueprintNativeEvent)
    void ReplicationPauseStart();
    
    UFUNCTION(BlueprintNativeEvent)
    void ReplicationPauseEnd();
    
    UFUNCTION(BlueprintNativeEvent)
    void Shooting();
    
    UFUNCTION(BlueprintNativeEvent)
    void BallsStopped();
};
