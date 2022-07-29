// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolUserWidgetBase.h"
#include "PoolPowerBarChalkWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolPowerBarChalkWidget : public UPoolUserWidgetBase
{
	GENERATED_BODY()
	
    
public:
    UFUNCTION(BlueprintCallable)
    void SetPower(float power);
    UFUNCTION(BlueprintCallable)
    void InitiateShot();
    UFUNCTION(BlueprintCallable)
    void InitiateChalking();
    UFUNCTION(BlueprintCallable)
    void SwitchAngle();
    UFUNCTION(BlueprintCallable)
    void PowerSliderGrabbed();
    UFUNCTION(BlueprintCallable)
    void PowerSliderUngrabbed();
    
    UFUNCTION(BlueprintNativeEvent)
    void Update(int shotsUntilRechalking);
    
    UFUNCTION(BlueprintNativeEvent)
    void ActivateAngleButton();
    
    UFUNCTION(BlueprintNativeEvent)
    void DeactivateAngleButton();
	
	
};
