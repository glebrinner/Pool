// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Engine.h"
#include "../Widgets/PoolBallInHandWidget.h"
#include "../Widgets/PoolCameraWidget.h"
#include "../Widgets/PoolJoystickWidget.h"
#include "../Widgets/PoolPowerBarChalkWidget.h"
#include "../Widgets/PoolUserBarWidgetBase.h"
#include "../Widgets/PoolMessageDeliveryWidget.h"
#include "../Widgets/PoolBaseShopWidget.h"
#include "PoolHUD.generated.h"


struct WidgetActivation
{
    bool game;
    bool pause;
    bool cameraSwitch;
    bool userBar;
    bool ballInHand;
    bool loading;
    EJoystickModes joystick;
    bool ballInHandButton;
    bool spect3DButton;
    bool spect2DButton;
    bool angle;
};

UCLASS()
class SEFAPOOL_API APoolHUD : public AHUD
{
	GENERATED_BODY()
    WidgetActivation act;
    bool pending;
    bool clearScreenOnce;
    TArray<UPoolUserWidgetBase*> widgetsSorted;
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    TSubclassOf<UPoolPowerBarChalkWidget> gameWidgetClass; // effect, power, chalk
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    TSubclassOf<UPoolUserWidgetBase> pauseWidgetClass; // pause
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    TSubclassOf<UPoolCameraWidget> cameraSwitchWidgetClass; // 3D, 2D
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    TSubclassOf<UPoolUserBarWidgetBase> userBarWidgetClass; // select cue (at the moment)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    TSubclassOf<UPoolBallInHandWidget> ballInHandWidgetClass; // ball in hand
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    TSubclassOf<UPoolJoystickWidget> joystickWidgetClass; // ball in hand
    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    //TSubclassOf<UPoolBaseShopWidget> shopWidgetClass; // ball in hand
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UMG")
    TSubclassOf<UPoolMessageDeliveryWidget> messagesWidgetClass; // ball in hand
    
	UPROPERTY(BlueprintReadOnly)
    UPoolPowerBarChalkWidget* gameWidget;
	UPROPERTY(BlueprintReadOnly)
    UPoolUserWidgetBase* pauseWidget;
    UPROPERTY(BlueprintReadOnly)
    UPoolCameraWidget* cameraSwitchWidget;
    UPROPERTY(BlueprintReadOnly)
    UPoolUserBarWidgetBase* userBarWidget;
    UPROPERTY(BlueprintReadOnly)
    UPoolBallInHandWidget* ballInHandWidget;
    UPROPERTY(BlueprintReadOnly)
    UPoolJoystickWidget* joystickWidget;
    UPROPERTY(BlueprintReadOnly)
    UPoolBaseShopWidget* shopWidget;
    UPROPERTY(BlueprintReadOnly)
    UPoolMessageDeliveryWidget* messagesWidget;

	APoolHUD();

	virtual void BeginPlay();
    
    void Init();

    void ActivateAimingUI(bool angle);

    void ActivateAimingWithFoulUI(bool angle);

    void ActivateSpectatingUI();
    
    void Activate3DSpectatingUI();
    void Activate2DSpectatingUI();
    void Activate2DAimingUI(bool angle);
    
    void Activate2DAimingWithFoulUI(bool angle);

    void ActivateBallInHandUI();

    void ActivateAfterShotUI();
    
    void ActivateLoading();
    
    void ShowShop();
    
    UPoolBaseShopWidget* GetShop();
    
    bool IsTouchCaught(ETouchIndex::Type FingerIndex, FVector2D Location);
    
    bool ConsumeTouch(ETouchIndex::Type FingerIndex, FVector2D Location);
    
    bool TouchReleased(ETouchIndex::Type FingerIndex, FVector2D Location);
    

private:
    void UpdateUI();
    
    
};
