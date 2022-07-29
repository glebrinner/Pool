
#include "PoolGameFlow.h"
#include "ObjectGatherer.h"
#include "Ball.h"
#include "PoolPlayer.h"

// Sets default values
APoolGameFlow::APoolGameFlow() :
    objects (nullptr),
    state   (GameFlowState_t::INIT),
    ballInHand    (false),
    grabbed2D (false)
{
	PrimaryActorTick.bCanEverTick = false;

}

void APoolGameFlow::BeginPlay()
{
	Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects)
}

void APoolGameFlow::SetPawn(APoolPlayer* pawn_)
{
    pawn = pawn_;
}

void APoolGameFlow::SetSpecPawn(AMainSpectatorPawn* SpecPawn_)
{
    SpecPawn = SpecPawn_;
}

void APoolGameFlow::Spectating()
{
    LOG_FUNC
    //PRINTF((TEXT("Flow: spectating")), Magenta)
    state = GameFlowState_t::SPECTATING;
    objects->cameraManager->SwitchToSpectating();
    objects->hud->ActivateSpectatingUI();
    // Set spectating camera to spectating position
    // show 3D and 2D buttons
    // Disable touch input in PC
}

void APoolGameFlow::SubSubAiming()
{
    LOG_FUNC
    objects->cameraManager->ShouldAiming = true;
    float cameraEffectTime = objects->cameraManager->SwitchToAiming();
    FTimerHandle UnusedHandle;
    GetWorldTimerManager().SetTimer(UnusedHandle, this, &APoolGameFlow::ActuallyStartAimingAfterCameraEffect, 1.0f, false, cameraEffectTime);
}

void APoolGameFlow::ActuallyStartAimingAfterCameraEffect()
{
    LOG_FUNC
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*> (UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (pc)
        pc->ActuallyStartAiming();
}


void APoolGameFlow::SubAiming()
{
    LOG_FUNC
    pawn->DeactivateWalkMode();
    
    /*if (!ballInHand)
    {*/
        SubAimingForced();
    /*}
    else
    {
        BallInHand();
        //ballInHand = false;
    }*/
    //PRINTF((TEXT("Flow: SubAiming %d"), ballInHand), Magenta)
}

void APoolGameFlow::SubAimingForced()
{
    LOG_FUNC
    if (grabbed2D)
        Camera2DAiming();
    else
    {
        bool needSubSubAiming = true;
        if (ballInHand)
        {
            state = GameFlowState_t::AIMING_HAND;
            if (objects->gi && objects->gi->data->settings->GetAutoTranslationBallInHand())
            {
                needSubSubAiming = false;
                SwitchBallInHand();
            }
            else
            {
                objects->hud->ActivateAimingWithFoulUI(false);
            }
        }
        else
        {
            state = GameFlowState_t::AIMING;
            objects->hud->ActivateAimingUI(false);
        }
        if (needSubSubAiming)
            SubSubAiming();
    }
    //PRINTF((TEXT("Flow: SubAiming forced")), Magenta)
}
void APoolGameFlow::Aiming(bool timed)
{
    LOG_FUNC
    if (timed)
    {
        FTimerHandle UnusedHandle;
        GetWorldTimerManager().SetTimer(UnusedHandle, this, &APoolGameFlow::SubAiming, 1.0f, false, 1.0f);
        PRINTF((TEXT("Flow: timed Aiming")), Magenta)
    }
    else
    {
        PRINTF((TEXT("Flow: Aiming")), Magenta)
        //SubAiming();
        objects->cameraManager->EngageFlowCallback();
    }
    
    // Activate PoolPlayer
    // Launch camera switch
}

void APoolGameFlow::BallInHand()
{
    //PRINTF((TEXT("Flow: Ball in hand")), Magenta)
    
    
    objects->cameraManager->SetFrom2D();
    objects->cameraManager->ActivateTopCamera();
    state = GameFlowState_t::BALL_IN_HAND;
    objects->hud->ActivateBallInHandUI();
    pawn->Deactivate();
    
    
    //state = GameFlowState_t::AIMING_HAND;
    //objects->hud->ActivateAimingWithFoulUI();
    // Aiming + ball in hand
}
void APoolGameFlow::SwitchBallInHand()
{
    PRINTF((TEXT("Flow: switch ball in hand")), Magenta)
    switch (state)
    {
        case GameFlowState_t::CAMERA_2D_AIMING:
            if (!ballInHand) break;
        case GameFlowState_t::AIMING_HAND:
            BallInHand();
            break;
        case GameFlowState_t::BALL_IN_HAND:
            if (!grabbed2D)
            {
                state = GameFlowState_t::AIMING_HAND;
                objects->hud->ActivateAimingWithFoulUI(false);
                SubSubAiming();
            }
            else
                Camera2DAiming();
        default:
            break;
    }
    // Move to 2D camera, touch input moves the ball
}
void APoolGameFlow::AfterShot(ABall* targetBall, FVector targetVec, float shotPower)
{
    LOG_FUNC
    //PRINTF((TEXT("Flow: after shot")), Magenta)
    objects->cameraManager->BrokerCameraSwitch();
    changingAngle = false;
    if (state == GameFlowState_t::CAMERA_2D_AIMING || grabbed2D)
    {
        Camera2DSpectating();
    }
    else
    {
        state = GameFlowState_t::AFTER_SHOT;
        objects->hud->ActivateSpectatingUI();
        //if (!pawn->IsElevated())
            objects->cameraManager->StartFollowing(targetBall, targetVec, shotPower);
    }
    // Touch input disabled, camera manager moves the cameras
}

void APoolGameFlow::Switch2D()
{
    PRINTF((TEXT("Flow: Switch2D")), Magenta)
    SetChangingAngle(false);
    switch (state)
    {
        case GameFlowState_t::AIMING:
        case GameFlowState_t::AIMING_HAND:
        case GameFlowState_t::CAMERA_3D_AIMING:
            objects->cameraManager->BrokerCameraSwitch();
            grabbed2D = true;
            if (pawn)
                pawn->DeactivateWalkMode();
            Camera2DAiming();
            break;
        case GameFlowState_t::SPECTATING:
        case GameFlowState_t::CAMERA_3D_SPECTATING:
        case GameFlowState_t::AFTER_SHOT:
            grabbed2D = true;
            if (pawn)
                pawn->DeactivateWalkMode();
            Camera2DSpectating();
            break;
        case GameFlowState_t::CAMERA_2D_AIMING:
            grabbed2D = false;
            SubAimingForced();
            break;
        case GameFlowState_t::CAMERA_2D_SPECTATING:
            grabbed2D = false;
            Spectating();
            break;
        default:
            UE_LOG(LogTemp,Error,TEXT("APoolGameFlow::Switch2D : default"))
            break;
    }
}

void APoolGameFlow::Switch3D()
{
    PRINTF((TEXT("Flow: Switch3D %d"), state), Magenta)
    SetChangingAngle(false);
    switch (state)
    {
        case GameFlowState_t::AIMING:
        case GameFlowState_t::AIMING_HAND:
        case GameFlowState_t::CAMERA_2D_AIMING:
            objects->cameraManager->BrokerCameraSwitch();
            Camera3DAiming();
            break;
        case GameFlowState_t::AFTER_SHOT:
        case GameFlowState_t::SPECTATING:
        case GameFlowState_t::CAMERA_2D_SPECTATING:
            Camera3DSpectating();
            break;
        case GameFlowState_t::CAMERA_3D_AIMING:
            if (pawn)
                pawn->ActivateWalkMode();
            SubAimingForced();
            break;
        case GameFlowState_t::CAMERA_3D_SPECTATING:
            Spectating();
            if (pawn)
                pawn->DeactivateWalkMode();
            break;
        default:
            break;
    }
}

void APoolGameFlow::Stop3D()
{
    PRINTF((TEXT("Flow: Stop3D")), Magenta)
    SetChangingAngle(false);
    switch (state)
    {
        case GameFlowState_t::CAMERA_3D_AIMING:
            if (pawn)
                pawn->DeactivateWalkMode();
            SubAimingForced();
            break;
        case GameFlowState_t::CAMERA_3D_SPECTATING:
            Spectating();
            if (pawn)
                pawn->DeactivateWalkMode();
            break;
        default:
            break;
    }
}

void APoolGameFlow::Camera2DAiming()
{
    if (!ballInHand)
    {
        PRINTF((TEXT("Flow: 2D aiming no ball in hand")), Magenta)
        objects->hud->Activate2DAimingUI(false);
    }
    else
    {
        PRINTF((TEXT("Flow: 2D aiming ball in hand")), Magenta)
        objects->hud->Activate2DAimingWithFoulUI(false);
    }
    ActuallyStartAimingAfterCameraEffect();
    pawn->SwitchElevationCam(false);
    pawn->Activate();
    state = GameFlowState_t::CAMERA_2D_AIMING;
    
    objects->cameraManager->SetFrom2D();
    objects->cameraManager->ActivateTopCamera();
}
void APoolGameFlow::Camera2DSpectating(bool justCamera)
{
    objects->cameraManager->SetFrom2D();
    grabbed2D = true;
    PRINTF((TEXT("Flow: 2D spectating")), Magenta)
    if (!justCamera)
    {
        state = GameFlowState_t::CAMERA_2D_SPECTATING;
        objects->hud->Activate2DSpectatingUI();
    }
    objects->cameraManager->ActivateTopCamera();
}
void APoolGameFlow::Camera3DAiming()
{
    grabbed2D = false;
    if (pawn)
        pawn->ActivateWalkMode();
    PRINTF((TEXT("Flow: 3D aiming")), Magenta)
    state = GameFlowState_t::CAMERA_3D_AIMING;
    objects->hud->Activate3DSpectatingUI();
    objects->cameraManager->FocusCameraToPlayer();
}
void APoolGameFlow::Camera3DSpectating()
{
    grabbed2D = false;
    PRINTF((TEXT("Flow: 3D spectating")), Magenta)
    state = GameFlowState_t::CAMERA_3D_SPECTATING;
    objects->hud->Activate3DSpectatingUI();
    pawn->ActivateWalkMode();
    objects->cameraManager->FocusCameraToPlayer();
}

void APoolGameFlow::SetBallInHand(bool val)
{
    ballInHand = val;
}

void APoolGameFlow::SetChangingAngle(bool val)
{
    changingAngle = val;
    
    pawn->SwitchElevationCam(val);
    switch (state)
    {
        case GameFlowState_t::AIMING:
            objects->hud->ActivateAimingUI(changingAngle);
            //pawn->SwitchElevationCam(val);
            break;
        case GameFlowState_t::AIMING_HAND:
            objects->hud->ActivateAimingWithFoulUI(changingAngle);
            //pawn->SwitchElevationCam(val);
            break;
        case GameFlowState_t::CAMERA_2D_AIMING:
            if (ballInHand)
                objects->hud->Activate2DAimingWithFoulUI(changingAngle);
            else
                objects->hud->Activate2DAimingUI(changingAngle);
            break;
        default:
            break;
    }
}
bool APoolGameFlow::GetChangingAngle()
{
    return changingAngle;
}
