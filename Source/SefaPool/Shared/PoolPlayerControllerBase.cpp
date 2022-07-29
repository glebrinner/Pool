// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolPlayerControllerBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "../Shared/SharedDefs.h"
#include "../Shared/PoolGameStateBase.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "PoolHUD.h"
#include "ObjectGatherer.h"
#include "PoolGameModeBase.h"
#include "PoolPlayerStateBase.h"
#include "SefaPool/8Ball/BallInfo.h"
#include "../Widgets/PoolSPUserBarWidget.h"
#include "SefaPool/8Ball/BallsGatherer/BallManagerBase.h"
#include "SefaPool/8Ball/Referee/RefereeBase.h"
#include "Net/UnrealNetwork.h"

APoolPlayerControllerBase::APoolPlayerControllerBase() :
    objects                 (nullptr),
    playingPawn             (nullptr),
    aiming                  (false),
    spectating              (false),
    touchPrevPos            (),
    pinchPrevPos            (),
    pinchPrevSize           (),
    touching                (false),
    pinching                (false),
    pendingStateChangeTimer (),
    nextState               (GameFlowState_t::INIT),
    ballHasBeenMoved        (),
    cuesReady               (),
    caughtFinger            (ETouchIndex::Touch1),
    caughtFinger2           (ETouchIndex::Touch1),
    lastPinchStartTime      (0.0f),
    lastTouchStartTime      (0.0f),
    shotStartTime           (0.0f),
    usingAngle              (),
    fingersRelayedToHUD     (),
    actuallyStartAimingOnce (false),
    playerIsChangingPower   (false),
    bSpectator              (false)
{
    PrimaryActorTick.bCanEverTick = true;
    bIsBallSeted = false;
    inPauseMenu = false;
}




void APoolPlayerControllerBase::BeginPlay()
{
    if (!this)
        ERROR_DEBUG_PRINT_FUNC
    Super::BeginPlay();
    //123 //123 PRINTF((TEXT(__FUNCSIG__)), Yellow)
    GATHER_OBJECT_GATHERER(objects)
    
    
    if (IsLocalPlayerController())
    {
        SetInputMode(FInputModeGameOnly());
        bShowMouseCursor = true;
        bEnableClickEvents = true;
        bEnableMouseOverEvents = true;
        playingPawn = dynamic_cast<APoolPlayer*>(GetPawn());
        objects->gameflow->SetPawn(playingPawn);
        if (!playingPawn)
            ERROR_DEBUG_PRINT_FUNC_INFO("PoolPlayer pawn not found nullptr")
        
        objects->cameraManager->Populate(this, playingPawn);
        if (objects->hud)
            objects->hud->Init();
        //objects->gameflow->Spectating();
        GetWorldTimerManager().SetTimer(initTimer,
                                        this,
                                        &APoolPlayerControllerBase::TimedServerPCReady,
                                        1.0f,
                                        true,
                                        1.0f);
        auto* gi = Cast<USefaPoolGameInstance>(GetGameInstance());
        if (gi)
        {
            gi->orientationManager->SwitchToOrientation(E_ScreenOrientation::ORIENTATION_ANY_LANDSCAPE, this);
            if (gi->data->settings->GetPreferredCameraMode() == EPreferredCameraMode::Cam2D)
                objects->gameflow->Camera2DSpectating(true);
            else
                objects->cameraManager->SwitchToSpectating();
        }
        
    }
    bAutoManageActiveCameraTarget = false;
    touchPrevPos = {};
    touching = false;
    pinching = false;
    actuallyStartAimingOnce = true;
}

void APoolPlayerControllerBase::TimedServerPCReady()
{
    /*if (objects->ballinfo->singlePosUpdates > 0)*/
    {
        GetWorldTimerManager().ClearTimer(initTimer);
        ServerPCReady();
        UE_LOG(LogTemp,Error,TEXT("PC READY"))
    }
}


void APoolPlayerControllerBase::DoublePinchTap()
{
    if (objects->gameflow->state == GameFlowState_t::CAMERA_2D_AIMING ||
        objects->gameflow->state == GameFlowState_t::CAMERA_2D_SPECTATING)
    {
        objects->cameraManager->Reset2DCam();
    }
    else
    if (objects->gameflow->state == GameFlowState_t::AIMING ||
        objects->gameflow->state == GameFlowState_t::AIMING_HAND ||
        objects->gameflow->state == GameFlowState_t::CAMERA_3D_SPECTATING ||
        objects->gameflow->state == GameFlowState_t::CAMERA_3D_AIMING)
    {
        playingPawn->ResetCameraViewMode(true);
    }
    
}

void APoolPlayerControllerBase::Tap()
{
    if (objects->gameflow->state == GameFlowState_t::CAMERA_2D_AIMING &&
        !objects->gameflow->GetChangingAngle() &&
        objects->gi->data->settings->GetTapToAim2D())
    {
        FVector touchLocation, touchDirection;
        DeprojectScreenPositionToWorld(touchPrevPos.X,
                                       touchPrevPos.Y,
                                       touchLocation,
                                       touchDirection);
        FVector2D tapPosOnTable = objects->physicsEngine->WorldToTableCoords(touchLocation);
        PRINTF((TEXT("TAP: %s"), *tapPosOnTable.ToString()), White)
        if ((2.0f*tapPosOnTable.X - 1.0f) * (2.0f*tapPosOnTable.X - 1.0f) < 1.125f * 1.125f &&
            (tapPosOnTable.Y - 1.0f) * (tapPosOnTable.Y - 1.0f) < 1.125f * 1.125f)
            playingPawn->RotateCueTo(touchLocation);
    }
}

void APoolPlayerControllerBase::Tick(float dt)
{
    TArray<ETouchIndex::Type> fingersToRemove;
    for (auto& finger: fingersRelayedToHUD)
    {
        FVector2D newPos;
        bool currentlyTouching = false;
        GetInputTouchState(finger, newPos.X, newPos.Y, currentlyTouching);
        
        if (currentlyTouching)
        {
            objects->hud->ConsumeTouch(finger, newPos);
        }
        else
        {
            objects->hud->TouchReleased(finger, newPos);
            fingersToRemove.Add(finger);
        }
    }
    for (auto& finger: fingersToRemove)
        fingersRelayedToHUD.Remove(finger);
    
    if (inPauseMenu) return;

    if (touching)
    {
        bool currentlyTouching = false;
        FVector2D newPos;
        GetInputTouchState(caughtFinger, newPos.X, newPos.Y, currentlyTouching);
        
        if (!currentlyTouching)
        {
            if (GetGameTimeSinceCreation() - lastTouchStartTime < objects->gi->data->settings->GetTapDelay())
            {
                touching = false;
                pinching = false;
                Tap();
                return;
            }
            touching = false;
            return;
        }
        if (fabs((newPos - touchPrevPos).Size()) < 1e-5f) return;
        if (objects->gameflow->state == GameFlowState_t::AIMING ||
            objects->gameflow->state == GameFlowState_t::CAMERA_2D_AIMING ||
            objects->gameflow->state == GameFlowState_t::AIMING_HAND)
        {
            FVector2D rotation(0.0f);
            if (!objects->gameflow->GetChangingAngle())
                rotation.X = (newPos - touchPrevPos).X * 0.05f * objects->gi->data->settings->GetFocusCameraHorizontalSensitivity()/* * orientation*/;
            else
                rotation.Y = (newPos - touchPrevPos).Y * 0.01f * objects->gi->data->settings->GetFocusCameraVerticalSensitivity()/* * orientation*/;
                
            playingPawn->RotateCue(rotation);
            
            if (!objects->gameflow->GetChangingAngle())
            {
                float z_adj = (newPos - touchPrevPos).Y * 0.01f * objects->gi->data->settings->GetFocusCameraVerticalSensitivity()/* * orientation*/;
                playingPawn->CameraAdjust(0.0f, z_adj);
            }
            
        }
        else
        if (objects->gameflow->state == GameFlowState_t::CAMERA_3D_AIMING ||
            objects->gameflow->state == GameFlowState_t::CAMERA_3D_SPECTATING)
        {
            /*playingPawn->AdjustWalkDist((newPos - touchPrevPos).Y * objects->settings->yTouchSensitivity * objects->settings->yAxisOrientation);*/
            playingPawn->CameraAdjust((newPos - touchPrevPos).X * 0.05f * objects->gi->data->settings->GetViewCameraHorizontalSensitivity()/* * orientation*/, 0.0f);
        }
        else
        if (objects->gameflow->state == GameFlowState_t::BALL_IN_HAND)
        {
            FVector touchLocationOld, touchLocationNew, touchDirection;
            DeprojectScreenPositionToWorld(touchPrevPos.X,
                                           touchPrevPos.Y,
                                           touchLocationOld,
                                           touchDirection);
            
            DeprojectScreenPositionToWorld(newPos.X,
                                           newPos.Y,
                                           touchLocationNew,
                                           touchDirection);
            
            FVector tmp = touchLocationNew - touchLocationOld;
            FVector2D vec = FVector2D(-tmp.Y, tmp.X) * objects->gi->data->settings->GetBallInHandSensitivity();
            BallInHandMoveBallIncremental(vec);
        }
        touchPrevPos = newPos;
    }
    else
    if (pinching)
    {
        
        bool currentlyTouching = false;
        FVector2D newPos;
        GetInputTouchState(caughtFinger, newPos.X, newPos.Y, currentlyTouching);
        bool currentlyTouching2 = false;
        FVector2D newPos2;
        GetInputTouchState(caughtFinger2, newPos2.X, newPos2.Y, currentlyTouching2);
        
        if (currentlyTouching != currentlyTouching2)
        {
            touching = true;
            pinching = false;
            if (currentlyTouching2)
                caughtFinger = caughtFinger2;
            touchPrevPos = !currentlyTouching2 ? newPos : newPos2;
            return;
        }
        else
        if (!currentlyTouching && !currentlyTouching2)
        {
            touching = false;
            pinching = false;
            return;
        }
        
        FVector2D newCenter = (newPos + newPos2) / 2.0f;
        float newSize = (newPos2 - newPos).Size();
        
        FVector2D move = newCenter - pinchPrevPos;
        float resize = newSize - pinchPrevSize;
        if (objects->gameflow->state == GameFlowState_t::AIMING ||
            objects->gameflow->state == GameFlowState_t::AIMING_HAND)
        {
            playingPawn->CameraAdjust(resize * objects->gi->data->settings->GetFocusPinchSensitivity()/* * orientation */, 0.0f);
        }
        else
        if (objects->gameflow->state == GameFlowState_t::CAMERA_2D_AIMING ||
            objects->gameflow->state == GameFlowState_t::CAMERA_2D_SPECTATING)
        { // FIX THIS
            objects->cameraManager->Move2DCam(move *
                                              FVector2D(2.0f * objects->gi->data->settings->GetViewCameraHorizontalSensitivity(),
                                                        2.0f * objects->gi->data->settings->GetViewCameraHorizontalSensitivity()),
                                              resize * 5.0f * objects->gi->data->settings->GetViewCameraHorizontalSensitivity());
        }
        else
        if (objects->gameflow->state == GameFlowState_t::CAMERA_3D_SPECTATING ||
            objects->gameflow->state == GameFlowState_t::CAMERA_3D_AIMING)
        { // FIX THIS
            playingPawn->AdjustWalkDist(resize * objects->gi->data->settings->GetViewCameraHorizontalSensitivity());
        }
        
        pinchPrevPos = newCenter;
        pinchPrevSize = newSize;
        touchPrevPos = newPos;
    }
}

void APoolPlayerControllerBase::ClientSetViewToTable_Implementation()
{
    LOG_FUNC
    if (!this || !objects->table)
        ERROR_DEBUG_PRINT_FUNC
    PRINTF((TEXT("ClientSetViewToTable")), White)
    nextState = GameFlowState_t::SPECTATING;
    ActivateFoulIfNeeded();
    if (objects->gameflow->state != GameFlowState_t::SPECTATING &&
        objects->gameflow->state != GameFlowState_t::CAMERA_2D_SPECTATING &&
        objects->gameflow->state != GameFlowState_t::CAMERA_3D_SPECTATING)
        objects->gameflow->Spectating();
    UpdateUserBarWidget();
    GetWorldTimerManager().SetTimer(pendingStateChangeTimer,
                                    this,
                                    &APoolPlayerControllerBase::ProceedToNextStageIfVisualizationHasEnded,
                                    1.1f,
                                    true,
                                    2.0f);
}

void APoolPlayerControllerBase::ServerPCReady_Implementation()
{
    if (!this || !objects->table)
        ERROR_DEBUG_PRINT_FUNC
    // GetWorld()->GetAuthGameMode<APoolGameModeBase>()->PCReady(this);
}

void APoolPlayerControllerBase::ActivateFoulIfNeeded()
{
    if (!PlayerState) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*>(PlayerState);
    if (!ps) return;
    if (ps->ballInHand)
    {
        objects->gameflow->SetBallInHand(true);
        DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_BALL_IN_HAND);
        UE_LOG(LogTemp,Error,TEXT("Ball in hand true"))
    }
    else
    {
        objects->gameflow->SetBallInHand(false);
        UE_LOG(LogTemp,Error,TEXT("Ball in hand false"))
    }
}

void APoolPlayerControllerBase::ClientStartPlay_Implementation()
{
    LOG_FUNC
    if (!this || !playingPawn)
        ERROR_DEBUG_PRINT_FUNC
    PRINTF((TEXT("ClientStartPlay")), White)
        
    
    nextState = GameFlowState_t::AIMING;
    actuallyStartAimingOnce = true;
    ActivateFoulIfNeeded();
    /*if (objects->gameflow->state != GameFlowState_t::SPECTATING &&
        objects->gameflow->state != GameFlowState_t::CAMERA_2D_SPECTATING &&
        objects->gameflow->state != GameFlowState_t::CAMERA_3D_SPECTATING)
        objects->gameflow->Spectating();*/
    UpdateUserBarWidget();
    GetWorldTimerManager().SetTimer(pendingStateChangeTimer,
                                    this,
                                    &APoolPlayerControllerBase::ProceedToNextStageIfVisualizationHasEnded,
                                    1.1f,
                                    true,
                                    2.0f);
}

void APoolPlayerControllerBase::Server_CalculateCurrentIdleDelay_Implementation()
{
    APoolGameModeBase* GM = Cast<APoolGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->Referee->OnPlayerStageStart();
    }
}

void APoolPlayerControllerBase::ProceedToNextStageIfVisualizationHasEnded()
{
    LOG_FUNC
    if (IsReady())
    {
        switch (nextState)
        {
            case GameFlowState_t::AIMING:
            {
                if (objects && objects->opponent)
                    objects->opponent->Disable();
                objects->gameflow->Aiming(false);
                GetWorldTimerManager().ClearTimer(pendingStateChangeTimer);
                shotStartTime = GetGameTimeSinceCreation();
                DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_YOUR_TURN);
                
                break;
            }
            case GameFlowState_t::SPECTATING:
            {
                PRINTF((TEXT("Timer spectating")), White)
                if (objects->gameflow->state != GameFlowState_t::SPECTATING &&
                    objects->gameflow->state != GameFlowState_t::CAMERA_2D_SPECTATING &&
                    objects->gameflow->state != GameFlowState_t::CAMERA_3D_SPECTATING)
                {
                    if (objects->gameflow->state != GameFlowState_t::CAMERA_3D_SPECTATING)
                        playingPawn->DeactivateWalkMode();
                    objects->gameflow->Spectating();
                }
                GetWorldTimerManager().ClearTimer(pendingStateChangeTimer);
                break;
            }
        }
    }else UE_LOG(LogTemp,Error,TEXT("Not ready"))
    UE_LOG(LogTemp,Error,TEXT("ProceedToNextStageIfVisualizationHasEnded"))
}

void APoolPlayerControllerBase::ActuallyStartAiming()
{
    LOG_FUNC
    if (!actuallyStartAimingOnce) return;
    
    Server_CalculateCurrentIdleDelay();
    if (objects && objects->hud && objects->hud->userBarWidget)
    {
        objects->hud->userBarWidget->StartingAiming();
    }
    actuallyStartAimingOnce = false;
}

void APoolPlayerControllerBase::ClientEndPlay(ABall* targetBall, FVector targetVec, float shotPower)
{
    LOG_FUNC
    //123 //123 PRINTF((TEXT(__FUNCSIG__)), Yellow)
    if (!this || !objects->cameraManager)
        ERROR_DEBUG_PRINT_FUNC
    objects->gameflow->AfterShot(targetBall, targetVec, shotPower);
    
    usingAngle = false;
}

void APoolPlayerControllerBase::ShotEnded()
{
    LOG_FUNC
    if (!this || !objects->cameraManager)
        ERROR_DEBUG_PRINT_FUNC
        //objects->gameflow->Spectating();

        UE_LOG(LogTemp, Error, TEXT("StopPlay"))
        //ServerStopCollectBallsInfo();

}


void APoolPlayerControllerBase::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindTouch(IE_Pressed, this, &APoolPlayerControllerBase::OnTouchPressed);
    //InputComponent->BindTouch(IE_Released, this, &APoolPlayerControllerBase::OnTouchReleased);
}


void APoolPlayerControllerBase::OnPlay()
{
    LOG_FUNC
    if (IsLocalPlayerController())
    {
        // ServerOnPlay();
    }
    PRINTF((TEXT("OnPlay: spectating = %d, playingPawn = %x, playingPawn->isActive = %d"), spectating, playingPawn, playingPawn->IsActive()), Green)
    if (!spectating && playingPawn && playingPawn->IsActive())
    {
        DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_STARTING_SHOT);
        objects->shotinfo->ResetForNextShot();
        playingPawn->Shoot(this);
        ServerOnPlayerStageEnd();
    }
}

void APoolPlayerControllerBase::ServerOnPlay_Implementation()
{
    OnPlay();
} 

void APoolPlayerControllerBase::OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (inPauseMenu) return;
    PRINTF((TEXT("TOUCH: %f %f"), Location.X, Location.Y), Red)
    if (fingersRelayedToHUD.Contains(FingerIndex))
    {
        
        PRINTF((TEXT("TOUCH: old hud capture")), Red)
        return;
    }
    if (objects->hud->IsTouchCaught(FingerIndex, FVector2D(Location.X, Location.Y)))
    {
        fingersRelayedToHUD.Add(FingerIndex);
        PRINTF((TEXT("TOUCH: new hud capture")), Red)
    }
    else
    if (!touching && !pinching)
    {
        
        PRINTF((TEXT("TOUCH: starting touching")), Red)
        touching = true;
        caughtFinger = FingerIndex;
        touchPrevPos = FVector2D(Location.X, Location.Y);
        lastTouchStartTime = GetGameTimeSinceCreation();
    }
    else
    if (!pinching)
    {
        PRINTF((TEXT("TOUCH: starting pinching")), Red)
        if (GetGameTimeSinceCreation() - lastPinchStartTime < objects->gi->data->settings->GetDoublePinchTapSpeed())
        {
            pinching = false;
            touching = false;
            DoublePinchTap();
            return;
        }
        pinching = true;
        touching = false;
        caughtFinger2 = FingerIndex;
        FVector2D newLoc = FVector2D(Location.X, Location.Y);
        pinchPrevPos = (touchPrevPos + newLoc) / 2.0f;
        pinchPrevSize = (touchPrevPos - newLoc).Size();
        lastPinchStartTime = GetGameTimeSinceCreation();
    }
}
void APoolPlayerControllerBase::OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location)
{
    /*if (touching && !pinching && FingerIndex == caughtFinger)
    {
        if (FingerIndex != caughtFinger) return;
        touching = false;
        playingPawn->OnTouchEnd(FingerIndex, Location);
    }
    else
    if (pinching && !touching && (FingerIndex == caughtFinger2 || FingerIndex == caughtFinger))
    {
        pinching = false;
        touching = true;
        touchPrevPos = FVector2D(Location.X, Location.Y);
        if (FingerIndex == caughtFinger)
        {
            caughtFinger = caughtFinger2;
            touchPrevPos = touchPrevPos + (pinchPrevPos - touchPrevPos) * 2.0f;
        }
    }*/
    
}

void APoolPlayerControllerBase::SetPower(float value)
{
    if (!spectating && playingPawn && playingPawn->IsActive())
    {
        const float a = 0.125f;
        float power = a * (pow((1/a + 1), value) - 1.0f);
        playingPawn->ChangePowerAbsolute(power);
    }
}

void APoolPlayerControllerBase::SetAimDisplacement(FVector2D disp)
{
    if (!spectating && playingPawn && playingPawn->IsActive())
    {
        playingPawn->MoveAimAbsolute(disp * FVector2D(1.0f, -1.0f) * objects->gi->data->settings->GetFocusJoystickSensitivity());
    }
}

FVector2D APoolPlayerControllerBase::GetAimDisplacement()
{
    if (!spectating && playingPawn && playingPawn->IsActive())
    {
        FVector2D aimDisp = playingPawn->GetAbsoluteAimDisplacement();
        return aimDisp;
    }
    return FVector2D();
}

void APoolPlayerControllerBase::EffectMovementHasBegun()
{
    if (playingPawn)
        playingPawn->BeginEffectCloseUp();
}

void APoolPlayerControllerBase::EffectMovementHasEnded()
{
    if (playingPawn)
        playingPawn->EndEffectCloseUp();
}

void APoolPlayerControllerBase::CurrentFrameNextBallsToHit(TArray<int32>& arr)
{
    
}


UCueComponent* APoolPlayerControllerBase::GetCueComponent()
{
    if (playingPawn)
        return playingPawn->GetCueComponent();
    else
        return nullptr;
}

void APoolPlayerControllerBase::Switch3DCamera()
{
    objects->gameflow->Switch3D();
}

void APoolPlayerControllerBase::Stop3DCamera()
{
    objects->gameflow->Stop3D();
}

void APoolPlayerControllerBase::Switch2DCamera()
{
    objects->gameflow->Switch2D();
    UE_LOG(LogTemp,Error,TEXT("APoolPlayerControllerBase::Switch2DCamera"))
}
void APoolPlayerControllerBase::SwitchBallInHand()
{
    objects->gameflow->SwitchBallInHand();
}

void APoolPlayerControllerBase::ApplyChalk()
{
    DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_APPLYING_CHALK);
    ApplyChalk_Server();
    objects->sound->EmitChalkSound(playingPawn->GetActorLocation());
}

void APoolPlayerControllerBase::ApplyChalk_Server_Implementation()
{
    if (!HasAuthority() || !objects || !objects->validation) return;
    
    objects->validation->ValidateChalkApplication(this);
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    if (ps) ps->SendImmediateMessage(E_MessageSeverity::SECONDARY, MESSAGE_CHALKED);
}

void APoolPlayerControllerBase::ChangeCue(int num)
{
    DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_CHANGING_CUE);
    
    ChangeCue_Server(num);
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    auto cue = GetCueComponent();
    if (ps &&
        ps->CanSelectCue(num) &&
        cue &&
        objects &&
        objects->hud &&
        objects->hud->GetShop())
    {
        cue->SelectCue(num);
        objects->hud->GetShop()->ChangeActiveItem(EConsumableType::CUE, num);
    }
}

void APoolPlayerControllerBase::ChangeChalk(int num)
{
    DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_CHANGING_CHALK);
    
    ChangeChalk_Server(num);
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    auto cue = GetCueComponent();
    if (ps &&
        ps->CanSelectChalk(num) &&
        cue &&
        objects &&
        objects->hud &&
        objects->hud->GetShop())
    {
        //cue->SelectChalk(num);
        objects->hud->GetShop()->ChangeActiveItem(EConsumableType::CHALK, num);
    }
}


void APoolPlayerControllerBase::ChangeCue_Server_Implementation(int num)
{
    if (!HasAuthority() || !objects || !objects->validation) return;
    objects->validation->ValidateCueChange(this, num);
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    if (ps)
    {
        if (ps->cueInfo.GetCueNum() == num)
            ps->SendImmediateMessage(E_MessageSeverity::SECONDARY, MESSAGE_CUE_CHANGED);
        else
            ps->SendImmediateMessage(E_MessageSeverity::SECONDARY, MESSAGE_CUE_NOT_CHANGED);
    }
}

void APoolPlayerControllerBase::ChangeChalk_Server_Implementation(int num)
{
    if (!HasAuthority() || !objects || !objects->validation) return;
    objects->validation->ValidateChalkChange(this, num);
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    if (ps)
    {
        if (ps->cueInfo.GetChalkNum() == num)
            ps->SendImmediateMessage(E_MessageSeverity::SECONDARY, MESSAGE_CHALK_CHANGED);
        else
            ps->SendImmediateMessage(E_MessageSeverity::SECONDARY, MESSAGE_CHALK_NOT_CHANGED);
    }
}

void APoolPlayerControllerBase::SwitchAngle()
{
    if (!objects || !objects->gameflow || playerIsChangingPower) return;
    objects->gameflow->SetChangingAngle(!objects->gameflow->GetChangingAngle());
    if (!usingAngle) DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_ANGLE_WARNING);
    usingAngle = true;
}

void APoolPlayerControllerBase::StartAngleChange()
{
    if (!objects || !objects->gameflow || playerIsChangingPower) return;
    objects->gameflow->SetChangingAngle(true);
    if (!usingAngle) DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_ANGLE_WARNING);
    usingAngle = true;
}

void APoolPlayerControllerBase::EndAngleChange()
{
    if (!objects || !objects->gameflow || playerIsChangingPower) return;
    objects->gameflow->SetChangingAngle(false);
    if (!usingAngle) DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_ANGLE_WARNING);
    usingAngle = true;
}

void APoolPlayerControllerBase::PowerChangeStarted()
{
    if (!objects || !objects->gameflow || playerIsChangingPower) return;
    objects->gameflow->SetChangingAngle(false);
    playerIsChangingPower = true;
}

void APoolPlayerControllerBase::PowerChangeEnded()
{
    if (!objects || !objects->gameflow || !playerIsChangingPower) return;
    playerIsChangingPower = false;
}


int APoolPlayerControllerBase::GetCurrentCue()
{
    if (!PlayerState) return 0;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    if (!ps) return 0;
    return ps->cueInfo.GetCueNum();
}
int APoolPlayerControllerBase::ShotsBeforeRechalking()
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    if (!ps) return 0;
    return ps->cueInfo.ShotsBeforeRechalking();
    /*if (playingPawn)
    {
        return playingPawn->IsChalked();
    }*/
    return false;
}



void APoolPlayerControllerBase::InitiateShot(FVector target, FVector2D offset, float power)
{
    
    LOG_FUNC
    if (ballHasBeenMoved && objects->gameflow->state == GameFlowState_t::AIMING_HAND)
        TransferBallInHandMovement_Server(objects->physicsEngine->GetBallLocation(0));
    ballHasBeenMoved = false;
    
    switch (objects->gi->data->settings->GetShotStrength())
    {
        case EShotStrength::Exponential:
        {
            float exp_coeff = 9.0f;
            power = 1.0f / (exp_coeff - 1.0f) * (pow(exp_coeff, power) - 1.0f);
            break;
        }
        case EShotStrength::Logarithmic:
        {
            float exp_coeff = 1.0f/9.0f;
            power = 1.0f / (exp_coeff - 1.0f) * (pow(exp_coeff, power) - 1.0f);
            break;
        }
        case EShotStrength::Linear:
            break;
    }
    
    InitiateShot_Server(target, offset, power, usingAngle || objects->gi->data->settings->GetHardMode());
    
    if (objects && objects->hud && objects->hud->userBarWidget)
    {
        if (!HasAuthority())
            objects->hud->userBarWidget->ReplicationPauseStart();
        else
            objects->hud->userBarWidget->Shooting();
    }
    
    objects->sound->StopSounds();
}

void APoolPlayerControllerBase::InitiateShot_Server_Implementation(FVector target, FVector2D offset, float power, bool usingAngle_)
{
    if (!HasAuthority() || !objects || !objects->validation) return;
    objects->validation->ValidateShot(this, target, offset, power, usingAngle_);
}
void APoolPlayerControllerBase::TransferBallInHandMovement_Server_Implementation(FVector target)
{
    if (!HasAuthority() || !objects || !objects->validation) return;
    UE_LOG(LogTemp, Warning, TEXT("Starting ball in hand movement validation"))
    objects->validation->ValidateBallInHandNewCueBallPos(this, target);
}

void APoolPlayerControllerBase::RechargeCue_Server_Implementation(int num)
{
    if (!HasAuthority() || !objects || !objects->validation) return;
    objects->validation->ValidateConsumableRecharge(this, EConsumableType::CUE, num);
}

void APoolPlayerControllerBase::RechargeChalk_Server_Implementation(int num)
{
    if (!HasAuthority() || !objects || !objects->validation) return;
    objects->validation->ValidateConsumableRecharge(this, EConsumableType::CHALK, num);
}

/*
void APoolPlayerControllerBase::Server_OnPlayTimeEnded_Implementation()
{
    //ClientEndPlay(nullptr,FVector::ZeroVector,0.f);
}*/

void APoolPlayerControllerBase::ShotTimeHasEnded_Implementation()
{
    CueTimeHasEnded_Body();
}

void APoolPlayerControllerBase::CueTimeHasEnded_Body()
{
    DisplayMessage(E_MessageSeverity::SECONDARY, MESSAGE_TIME_ENDED);
    objects->gameflow->Spectating();
    playingPawn->Deactivate();
    nextState = GameFlowState_t::SPECTATING;
    GetWorldTimerManager().SetTimer(pendingStateChangeTimer,
                                    this,
                                    &APoolPlayerControllerBase::ProceedToNextStageIfVisualizationHasEnded,
                                    1.1f,
                                    true,
                                    2.0f);
}

void APoolPlayerControllerBase::ClientTravelToLevel_Implementation(const FName& level, const FString& widget, const FString& options)
{
    FTimerHandle t;
    USefaPoolGameInstance* gi = Cast<USefaPoolGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        ULoadingWidget* wg = Cast<ULoadingWidget>(gi->widgetManager->GetWidget("Loading"));
        if (wg)
            wg->ChangeVisibleText("Loading");
        else
            PRINTF((TEXT("Widget was nullptr")), Red);
    }
    else
    {
        PRINTF((TEXT("GI was nullptr")), Red);
    }
    GetWorldTimerManager().SetTimer(t,
                                    [this, level, widget, options] {TimedTravelToLevel(level, widget, options);},
                                    5.0f,
                                    false,
                                    5.0f);
}

void APoolPlayerControllerBase::TimedTravelToLevel(FName level, FString widget, FString options)
{
    if (this && GetWorld())
    {
        PRINTF((TEXT("travel to %s with widget %s"), *level.ToString(), *widget), Red);
        FString user_id = Cast<APoolPlayerStateBase>(PlayerState)->user_id;
        FString access_token = Cast<UPoolClientGameInstance>(GetGameInstance())->backend_data.access_token;
        
        if (Cast<USefaPoolGameInstance>(GetGameInstance())->widgetManager->ShowWidget(widget)) {
            PRINTF((TEXT("Failed to show widget %s"), *widget), Red);
        }

        USefaPoolGameInstance* gi = Cast<USefaPoolGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        if (gi) 
        {
            gi->OpenLevel(level.ToString(), options + FString::Format(TEXT("user_id={0}?access_token={1}"), { user_id, access_token }));
        }
        else
        {
            UGameplayStatics::OpenLevel(GetWorld(), level, true, options + FString::Format(TEXT("user_id={0}?access_token={1}"), { user_id, access_token })); // Adding access token and user_id to any options
        }
    }
    else
    {
        PRINTF((TEXT("Class pointer: %s, GetWorld is: %s"), this ? "true" : "false", GetWorld() ? "true" : "false"), Red);
    }
}

bool APoolPlayerControllerBase::IsReady()
{
    if (!objects || !objects->ballinfo || !objects->ballvisualization) return false;
    return objects->ballinfo->singlePosUpdates > 0 && !objects->ballvisualization->IsVisualize() && cuesReady && bIsBallSeted;
}

void APoolPlayerControllerBase::UpdateUserBarWidget()
{
    bool ok = true;
    UE_LOG(LogTemp,Error,TEXT("APoolPlayerControllerBase::UpdateUserBarWidget"))
    if (ok && !objects) ok = false;
    if (ok && !objects->hud) ok = false;
    if (ok && !objects->hud->userBarWidget) ok = false;
    UPoolUserBarWidgetBase* wid = dynamic_cast<UPoolUserBarWidgetBase*> (objects->hud->userBarWidget);
    if (ok && !wid) ok = false;
    
    if (!ok) return;
    
    APoolGameStateBase* gs = GetWorld()->GetGameState<APoolGameStateBase>();
    if (ok && !gs) ok = false;
    auto balls = gs->ballsInGame;
    
    TArray<int32> thisNextBalls;
    
    for (int i = 1; i < 16; i++)
    {
        if (!balls[i])
        {
            thisNextBalls.Add(i);
        }
    }
    UE_LOG(LogTemp,Error,TEXT("Send %i target balls"),thisNextBalls.Num())
    wid->UpdateUserTargetBalls(thisNextBalls);
}

void APoolPlayerControllerBase::BallInHandMoveBallIncremental(FVector2D vec)
{
    if (playingPawn &&
        objects &&
        objects->gameflow &&
        objects->gameflow->state == GameFlowState_t::BALL_IN_HAND)
    {
        if (!objects->physicsEngine->GetMovingBall())
        {
            playingPawn->BallInHandMoveBallIncremental(vec);
        }
       else
       {
           APoolGameStateBase* gs = GetWorld()->GetGameState<APoolGameStateBase>();
           if (!gs) return;
    
           FVector prevBallPos(objects->physicsEngine->GetBallLocation(objects->physicsEngine->GetMovingBall()));
    
           FVector dirVector = gs->GetFoulBallDefaultReturnLocationOnTable() - gs->GetCueBallDefaultLocationOnTable();
           dirVector.Z = 0;
           dirVector.Normalize();
           FVector orthogonalDir = FVector::CrossProduct(dirVector, FVector(0.0f, 0.0f, 1.0f));
    
           float speedCoeff = 0.5f;
    
           FVector targetPos = prevBallPos + (-dirVector * vec.Y - orthogonalDir * vec.X) * speedCoeff;
    
           objects->physicsEngine->ValidateCueBallLinearMovement(targetPos);
       }
        ballHasBeenMoved = true;
    }
}


void APoolPlayerControllerBase::Adjust3DCam(FVector2D amount)
{
    if (playingPawn &&
        objects &&
        objects->gameflow &&
        (objects->gameflow->state == GameFlowState_t::CAMERA_3D_AIMING ||
         objects->gameflow->state == GameFlowState_t::CAMERA_3D_SPECTATING ))
    {
        playingPawn->AdjustWalkCamera(amount * objects->gi->data->settings->GetJoystick3DSensitivity());
    }
}

void APoolPlayerControllerBase::InitShop()
{
    if (objects &&
        objects->hud &&
        objects->hud->GetShop() &&
        this &&
        !this->cuesReady)
    {
        //objects->hud->GetShop()->Refresh();
        cuesReady = true;
        UE_LOG(LogTemp,Error,TEXT("Cues is ready"))
    }
    else
    {
        PRINTF((TEXT("Error when uploading cues to store")), Green);
        PRINT_BOOL(objects);
        PRINT_BOOL(objects->hud);
        PRINT_BOOL(objects->hud->GetShop());
        PRINT_BOOL(this);
    }
}

void APoolPlayerControllerBase::ClientSetBallManagerSettings_Implementation(uint8 FirstBallIndex,uint8 BallCountInGame)
{
    if (HasAuthority()) return;
    UE_LOG(LogTemp,Error,TEXT("Client set ball"))
    objects->BallManager->FirstBallIndex = FirstBallIndex;
    objects->BallManager->BallsCountInGame = BallCountInGame;
    objects->BallManager->SetBallsForGM();
    bIsBallSeted = true;
    objects->ballvisualization->BallCountInGame = BallCountInGame;
}

void APoolPlayerControllerBase::ShowShop()
{
    if (objects &&
        objects->hud &&
        cuesReady)
    {
        objects->hud->ShowShop();
        PRINTF((TEXT("Showing shop")), Red)
    }
}

void APoolPlayerControllerBase::UpdateShop_Implementation(EConsumableType type, int num, int durability)
{
    if (!objects ||
        !objects->hud ||
        !objects->hud->GetShop() ||
        !this)
    {
        PRINT_BOOL(objects);
        PRINT_BOOL(objects->hud);
        PRINT_BOOL(objects->hud->GetShop());
        PRINT_BOOL(this);
        
        objects->hud->GetShop()->UpdateItemDurability(type,
                                                       num,
                                                       durability);
    }
    

}

void APoolPlayerControllerBase::ActivateCue_Implementation(int idx)
{
    if (!objects ||
        !objects->hud ||
        !objects->hud->GetShop() ||
        !this)
    {
        PRINT_BOOL(objects);
        PRINT_BOOL(objects->hud);
        PRINT_BOOL(objects->hud->GetShop());
        PRINT_BOOL(this);
    }
    
    //objects->hud->GetShop()->ActivateCueByIndex(idx);

}

UPoolPowerBarChalkWidget* APoolPlayerControllerBase::GetGameWidget()
{
    if (!objects || !objects->hud || !objects->hud->gameWidget)
        return nullptr;
    return objects->hud->gameWidget;
}

float APoolPlayerControllerBase::GetCueAngle()
{
    if (this && playingPawn)
        return playingPawn->GetCueAngle();
    return 0.0f;
}

void APoolPlayerControllerBase::ElevateCue(float angle)
{
    FVector2D rotation(0.0f);
    rotation.Y = angle;

    playingPawn->RotateCue(rotation);
}

bool APoolPlayerControllerBase::IsAngleChanging()
{
    if (this)
        return usingAngle;
    return false;
}


void APoolPlayerControllerBase::PauseMenuEntered()
{
    if (this) inPauseMenu = true;
}
void APoolPlayerControllerBase::PauseMenuExited()
{
    if (this) inPauseMenu = false;
}

void APoolPlayerControllerBase::ChangeCloth(E_TableClothColors color)
{
    if (objects && objects->table)
        objects->table->ApplyMaterial(color);
}

void APoolPlayerControllerBase::ChangeClothColor(FLinearColor color, float adj)
{
    if (objects && objects->table)
        objects->table->ApplyColor(color, adj);
}

void APoolPlayerControllerBase::ServerOnPlayerStageEnd_Implementation()
{
    APoolGameModeBase* GM = Cast<APoolGameModeBase>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        GM->Referee->OnPlayerStageEnd();
    }
}

float APoolPlayerControllerBase::GetShotTimeLeft()
{
    if (!PlayerState) return 0.0f;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    if (!ps || !objects->gameflow ||
        !(objects->gameflow->state == GameFlowState_t::AIMING ||
          objects->gameflow->state == GameFlowState_t::AIMING_HAND ||
          objects->gameflow->state == GameFlowState_t::CAMERA_2D_AIMING ||
          objects->gameflow->state == GameFlowState_t::CAMERA_3D_AIMING ||
          objects->gameflow->state == GameFlowState_t::BALL_IN_HAND)) return 0.0f;
    //return (GetGameTimeSinceCreation() - shotStartTime) / ps->cueInfo.GetTime();
    return ps->cueInfo.GetTime() - (GetGameTimeSinceCreation() - shotStartTime);
}

UFUNCTION(BlueprintCallable)
float APoolPlayerControllerBase::GetTotalShotTime()
{
    if (!PlayerState) return 0.0f;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayerState);
    if (!ps || !objects->gameflow ||
        !(objects->gameflow->state == GameFlowState_t::AIMING ||
          objects->gameflow->state == GameFlowState_t::AIMING_HAND ||
          objects->gameflow->state == GameFlowState_t::CAMERA_2D_AIMING ||
          objects->gameflow->state == GameFlowState_t::CAMERA_3D_AIMING ||
          objects->gameflow->state == GameFlowState_t::BALL_IN_HAND)) return 0.0f;
    //return (GetGameTimeSinceCreation() - shotStartTime) / ps->cueInfo.GetTime();
    return ps->cueInfo.GetTime();
}

void APoolPlayerControllerBase::DisplayMessage(E_MessageSeverity sev, uint16 messageId)
{
    if (this &&
        objects &&
        objects->hud &&
        objects->hud->messagesWidget)
        objects->hud->messagesWidget->EnqueueMessageForDisplay(sev, messageId);
}

void APoolPlayerControllerBase::PossessBySpectatorClass()
{
    SpawnSpectatorPawn();
    BeginSpectatingState();
}

void APoolPlayerControllerBase::ClientDrawDebugLine_Implementation(FVector TraceStart, FVector TraceEnd)
{
    // DrawDebugBox(GetWorld(),TraceEnd,FVector(1.f,1.f,10.f),FColor::Green,false,10.f);
    DrawDebugLine(GetWorld(),TraceStart,TraceEnd,FColor::Red,false,10.f,0,.3f);
    // UE_LOG(LogTemp,Error,TEXT("Client draw line"))
}

void APoolPlayerControllerBase::ClientDrawDebug_Implementation(FVector DrawStart, FVector DrawEnd)
{
    DrawDebugSphere(GetWorld(),DrawStart,5.f,12,FColor::Red,false,5.f);
    DrawDebugSphere(GetWorld(),DrawEnd,5.f,12,FColor::Green,false,5.f);
}

void APoolPlayerControllerBase::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME_CONDITION(APoolPlayerControllerBase,bSpectator,COND_OwnerOnly)
}
