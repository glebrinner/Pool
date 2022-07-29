// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolUserWidgetBase.h"
#include "PoolJoystickWidget.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum EJoystickModes
{
    JOYSTICK_MODE_AIMING       = 1 UMETA(DisplayName = "Aiming"),
    JOYSTICK_MODE_3D           = 2 UMETA(DisplayName = "3D"),
    JOYSTICK_MODE_BALL_IN_HAND = 3 UMETA(DisplayName = "BallInHand"),
    JOYSTICK_MODE_DISABLED     = 0 UMETA(DisplayName = "Disabled"),
};

UCLASS()
class SEFAPOOL_API UPoolJoystickWidget : public UPoolUserWidgetBase
{
	GENERATED_BODY()
	
    
public:
    UPROPERTY(BlueprintReadWrite)
    TEnumAsByte<EJoystickModes> mode;
    UPoolJoystickWidget();
    
    UFUNCTION(BlueprintCallable)
    void RegisterJoystickMovement(FVector2D newPos, bool justStarted);
    UFUNCTION(BlueprintCallable)
    void RegisterJoystickRelease();
    
    UFUNCTION(BlueprintCallable)
    FVector2D GetRelaxedJoystickPosition();
    
    void ChangeMode(EJoystickModes mode_);
    
    UFUNCTION(BlueprintNativeEvent)
    void ChangeModeBP();

    UFUNCTION(BlueprintCallable)
        void StartAngleChange();

    UFUNCTION(BlueprintCallable)
        void EndAngleChange();
};
