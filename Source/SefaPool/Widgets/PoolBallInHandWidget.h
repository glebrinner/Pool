// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolUserWidgetBase.h"
#include "PoolBallInHandWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolBallInHandWidget : public UPoolUserWidgetBase
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void InitiateSwitchBallInHand();
    
    
    UFUNCTION(BlueprintNativeEvent)
    void ActivateButton();
    
    UFUNCTION(BlueprintNativeEvent)
    void DeactivateButton();
	
	
};
