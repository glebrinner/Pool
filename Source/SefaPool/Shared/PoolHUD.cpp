// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolHUD.h"
#include "SharedDefs.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"

APoolHUD::APoolHUD() :
    act({}),
    pending            (false),
    clearScreenOnce    (false),
    widgetsSorted      (),
    gameWidget         (nullptr),
    pauseWidget        (nullptr),
    cameraSwitchWidget (nullptr),
    userBarWidget      (nullptr),
    ballInHandWidget   (nullptr),
    joystickWidget     (nullptr)
{
    PrimaryActorTick.bCanEverTick = false;
}

void APoolHUD::BeginPlay()
{
    Super::BeginPlay();
    Init();
}


void APoolHUD::Init()
{
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi) return;
#define CREATE_WIDGET_AND_CHECK(w, type) \
    if (!w##Widget && w##WidgetClass) \
w##Widget = dynamic_cast<type*> (gi->widgetManager->CreateTransientWidget(w##WidgetClass, GetWorld(), FString("game_") + TEXT(#w))); \
    else if (!w##Widget)\
        PRINTF((TEXT("HUD:: " #w "WidgetClass is null")), Red) \
    if (!w##Widget) \
        PRINTF((TEXT("HUD:: Failed to create " #w "Widget" )), Red)
    CREATE_WIDGET_AND_CHECK(game,         UPoolPowerBarChalkWidget)
    CREATE_WIDGET_AND_CHECK(pause,        UPoolUserWidgetBase)
    CREATE_WIDGET_AND_CHECK(cameraSwitch, UPoolCameraWidget)
    CREATE_WIDGET_AND_CHECK(userBar,      UPoolUserBarWidgetBase)
    CREATE_WIDGET_AND_CHECK(ballInHand,   UPoolBallInHandWidget)
    CREATE_WIDGET_AND_CHECK(joystick,     UPoolJoystickWidget)
    //CREATE_WIDGET_AND_CHECK(shop,         UPoolInGameShopWidget)
    CREATE_WIDGET_AND_CHECK(messages,     UPoolMessageDeliveryWidget)
    if (gi)
        shopWidget = dynamic_cast<UPoolBaseShopWidget*> (gi->widgetManager->GetWidget("InGameShop"));
    
    //if (shopWidget) shopWidget->AddToViewport(20);
    
#undef CREATE_WIDGET_AND_CHECK
}

void APoolHUD::ActivateAimingUI(bool angle)
{
    act.game             = true;
    act.pause            = true;
    act.cameraSwitch     = true;
    act.userBar          = true;
    act.ballInHand       = false;
    act.joystick         = JOYSTICK_MODE_AIMING;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = false;
    act.angle            = angle;
    UpdateUI();
}

void APoolHUD::ActivateAimingWithFoulUI(bool angle)
{
    act.game             = true;
    act.pause            = true;
    act.cameraSwitch     = true;
    act.userBar          = true;
    act.ballInHand       = true;
    act.joystick         = JOYSTICK_MODE_AIMING;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = false;
    act.angle            = angle;
    UpdateUI();
}

void APoolHUD::ActivateSpectatingUI()
{
    act.game             = false;
    act.pause            = true;
    act.cameraSwitch     = true;
    act.userBar          = true;
    act.ballInHand       = false;
    act.joystick         = JOYSTICK_MODE_DISABLED;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = false;
    act.angle            = false;
    UpdateUI();
}

void APoolHUD::Activate3DSpectatingUI()
{
    act.game             = false;
    act.pause            = true;
    act.cameraSwitch     = true;
    act.userBar          = true;
    act.ballInHand       = false;
    act.joystick         = JOYSTICK_MODE_3D;
    act.ballInHandButton = false;
    act.spect3DButton    = true;
    act.spect2DButton    = false;
    act.angle            = false;
    UpdateUI();
}

void APoolHUD::Activate2DSpectatingUI()
{
    act.game             = false;
    act.pause            = true;
    act.cameraSwitch     = true;
    act.userBar          = true;
    act.ballInHand       = false;
    act.joystick         = JOYSTICK_MODE_DISABLED;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = true;
    act.angle            = false;
    UpdateUI();
}

void APoolHUD::Activate2DAimingWithFoulUI(bool angle)
{
    act.game             = true;
    act.pause            = true;
    act.cameraSwitch     = true;
    act.userBar          = true;
    act.ballInHand       = true;
    act.joystick         = JOYSTICK_MODE_AIMING;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = true;
    act.angle            = angle;
    UpdateUI();
}

void APoolHUD::Activate2DAimingUI(bool angle)
{
    act.game             = true;
    act.pause            = true;
    act.cameraSwitch     = true;
    act.userBar          = true;
    act.ballInHand       = false;
    act.joystick         = JOYSTICK_MODE_AIMING;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = true;
    act.angle            = angle;
    UpdateUI();
}

void APoolHUD::ActivateBallInHandUI()
{
    act.game             = false;
    act.pause            = true;
    act.cameraSwitch     = false;
    act.userBar          = true;
    act.ballInHand       = true;
    act.joystick         = JOYSTICK_MODE_DISABLED;
    act.ballInHandButton = true;
    act.spect3DButton    = false;
    act.spect2DButton    = false;
    act.angle            = false;
    UpdateUI();
}

void APoolHUD::ActivateAfterShotUI()
{
    act.game             = false;
    act.pause            = true;
    act.cameraSwitch     = false;
    act.userBar          = true;
    act.ballInHand       = false;
    act.joystick         = JOYSTICK_MODE_DISABLED;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = false;
    act.angle            = false;
    UpdateUI();
}

void APoolHUD::ActivateLoading()
{
    act.game             = false;
    act.pause            = false;
    act.cameraSwitch     = false;
    act.userBar          = false;
    act.ballInHand       = false;
    act.joystick         = JOYSTICK_MODE_DISABLED;
    act.ballInHandButton = false;
    act.spect3DButton    = false;
    act.spect2DButton    = false;
    act.angle            = false;
    UpdateUI();
}
UPoolBaseShopWidget* APoolHUD::GetShop()
{
    if (!shopWidget)
    {
        USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
        if (gi)
            shopWidget = dynamic_cast<UPoolBaseShopWidget*> (gi->widgetManager->GetWidget("InGameShop"));
    }
    return shopWidget;
}

void APoolHUD::ShowShop()
{
    UPoolBaseShopWidget* shop = GetShop();
    if (shop)
        shop->Show();
}

void APoolHUD::UpdateUI()
{
    if (!clearScreenOnce)
    {
        clearScreenOnce = true;
        //UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
        USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
        gi->widgetManager->HideWidget("Loading");
        
        int zOrder = 0;
#define ADD_W_(name, zo)\
name##Widget->AddToViewport(zo); \
if (dynamic_cast<UPoolUserWidgetBase*>(name##Widget))\
widgetsSorted.Insert(dynamic_cast<UPoolUserWidgetBase*>(name##Widget), 0);
#define ADD_W(name)\
ADD_W_(name, zOrder); zOrder++;
        ADD_W(game)
        ADD_W(joystick)
        ADD_W(userBar)
        ADD_W(cameraSwitch)
        ADD_W(ballInHand)
        ADD_W_(pause, 10)
        ADD_W(messages)
#undef ADD_W
#undef ADD_W_
        
    }
    
    if (gameWidget)
    {
        if (act.game)
        {
            gameWidget->EnterEnabledMode();
            if (act.angle)
                gameWidget->ActivateAngleButton();
            else
                gameWidget->DeactivateAngleButton();
        }
        else gameWidget->EnterDisabledMode();
    }
    
    if (cameraSwitchWidget)
    {
        if (act.cameraSwitch) cameraSwitchWidget->EnterEnabledMode();
        else cameraSwitchWidget->EnterDisabledMode();
        
        if (act.spect3DButton)
            cameraSwitchWidget->Activate3DButton();
        else
        if (act.spect2DButton)
            cameraSwitchWidget->Activate2DButton();
        else
            cameraSwitchWidget->DeactivateButtons();
    }
    
    if (ballInHandWidget)
    {
        if (act.ballInHand) ballInHandWidget->EnterEnabledMode();
        else ballInHandWidget->EnterDisabledMode();
        
        if (act.ballInHandButton)
            ballInHandWidget->ActivateButton();
        else
            ballInHandWidget->DeactivateButton();
    }
    
    if (joystickWidget)
    {
        joystickWidget->ChangeMode(act.joystick);
    }
}

bool APoolHUD::IsTouchCaught(ETouchIndex::Type FingerIndex, FVector2D Location)
{
    
    for (auto& widg : widgetsSorted)
        if (widg->TouchStarted_Custom(FingerIndex, Location))
            return true;
    return false;
}

bool APoolHUD::ConsumeTouch(ETouchIndex::Type FingerIndex, FVector2D Location)
{
    for (auto& widg : widgetsSorted)
        if (widg->TouchMoved_Custom(FingerIndex, Location))
            return true;
    return false;
}

bool APoolHUD::TouchReleased(ETouchIndex::Type FingerIndex, FVector2D Location)
{
    for (auto& widg : widgetsSorted)
        if (widg->TouchEnded_Custom(FingerIndex, Location))
            return true;
    return false;
}
