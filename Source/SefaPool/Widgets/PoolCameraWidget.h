// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolUserWidgetBase.h"
#include "PoolCameraWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolCameraWidget : public UPoolUserWidgetBase
{
	GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void InitiateSwitch2D();
    
    UFUNCTION(BlueprintCallable)
    void InitiateSwitch3D();
    
    UFUNCTION(BlueprintCallable)
    void Stop3D();
    
    UFUNCTION(BlueprintCallable)
    void RegisterJoystickMovement(FVector2D newPos);
    
    
    UFUNCTION(BlueprintNativeEvent)
    void Activate2DButton();
    
    UFUNCTION(BlueprintNativeEvent)
    void Activate3DButton();
    
    UFUNCTION(BlueprintNativeEvent)
    void DeactivateButtons();
	
};
