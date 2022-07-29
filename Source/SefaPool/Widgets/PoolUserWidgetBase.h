// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Shared/CueComponent.h"
#include "PoolBaseWidget.h"
#include "PoolUserWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolUserWidgetBase : public UPoolBaseWidget
{
	GENERATED_BODY()
    
    FCueInfo cueInfo;
    
public:
	UFUNCTION(BlueprintNativeEvent)
    void EnterDisabledMode();
    
    UFUNCTION(BlueprintNativeEvent)
    void EnterEnabledMode();
    
    UFUNCTION(BlueprintNativeEvent)
    bool TouchStarted_Custom(ETouchIndex::Type FingerIndex, FVector2D Location);
    
    UFUNCTION(BlueprintNativeEvent)
    bool TouchMoved_Custom(ETouchIndex::Type FingerIndex, FVector2D Location);
    
    UFUNCTION(BlueprintNativeEvent)
    bool TouchEnded_Custom(ETouchIndex::Type FingerIndex, FVector2D Location);
    
    UFUNCTION(BlueprintCallable)
    float GetCuePower(int num);
    UFUNCTION(BlueprintCallable)
    float GetCueEffect(int num);
    UFUNCTION(BlueprintCallable)
    float GetCueTime(int num);
	
};
