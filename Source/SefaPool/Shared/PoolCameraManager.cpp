// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolCameraManager.h"
#include "SharedDefs.h"
#include "Table.h"
#include "Ball.h"
#include "PoolPlayerControllerBase.h"
#include "PoolSpectatorPawn.h"
#include "ObjectGatherer.h"
#include "PoolPlayer.h"
// Sets default values
APoolCameraManager::APoolCameraManager():
    root                      (CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"))),
    camera2D                  (CreateDefaultSubobject<UCameraComponent>(TEXT("Camera2D"))),
    topCamera                 (CreateDefaultSubobject<UCameraComponent>(TEXT("TopCamera"))),
    cameraFollowShot          (CreateDefaultSubobject<UCameraComponent>(TEXT("CameraFollow"))),
    objects                   (nullptr),
    blendCamera               (nullptr),
    player                    (nullptr),
    targetBall                (nullptr),
    pc                        (nullptr),
    followTime                (5.0f),
    followTimer               (),
    followTargetPos           (),
    usingTableCamera          (false),
    flowCameraCallbackEngaged (false),
    from2D                    (false),
    cameraSwitchBrokered      (false),
    instantFollow             (false)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    alreadySwitchedToSpectating = false;
}

// Called when the game starts or when spawned
void APoolCameraManager::BeginPlay()
{
    Super::BeginPlay();

    GATHER_OBJECT_GATHERER(objects)
    blendCamera = dynamic_cast<ACameraActor*> (GetWorld()->SpawnActor(ACameraActor::StaticClass()));
    blendCamera->GetCameraComponent()->SetConstraintAspectRatio(false);
    
    cameraSwitchBrokered = false;
}

// Called every frame
void APoolCameraManager::Tick(float DeltaTime)
{
    if ( pc && !pc->IsLocalPlayerController()) return;
    Super::Tick(DeltaTime);
}

void APoolCameraManager::Populate(APoolPlayerControllerBase* pc_,
                                  APoolPlayer* player_)
{
    pc = pc_;
    player = player_;
}

void APoolCameraManager::StartFollowing(ABall* targetBall_, FVector targetVector_, float shotPower_)
{
    LOG_FUNC
    if (!this)
        ERROR_DEBUG_PRINT_FUNC
    if (!pc->IsLocalPlayerController()) return;
    /*if (shotPower_ > 0.6f)
    {
        state = CameraState_t::FOLLOW;
        EndFollowAndSwitchTo2D();
        return;
    }*/
    
    if ((objects && objects->gi &&
         objects->gi->data->settings->GetCameraMoveMode() != ECameraMoveMode::None) &&
        (!instantFollow &&
        targetBall_ &&
        objects->tableCameras &&
        objects->tableCameras->ProcessShot(targetBall_, targetVector_)))
    {
        pc->SetViewTargetWithBlend(objects->tableCameras, 0.0f);
        usingTableCamera = true;
        GetWorldTimerManager().SetTimer(followTimer, this, &APoolCameraManager::EndFollowAndSwitchTo2D, followTime, false, followTime);
        return;
    }
    else
    {
        EndFollowAndSwitchTo2D();
    }
}

void APoolCameraManager::SwitchToSpectating()
{
    LOG_FUNC
    if (!this)
        ERROR_DEBUG_PRINT_FUNC
    if (!pc->IsLocalPlayerController()) return;
    ResetSpectatorCamera();
    alreadySwitchedToSpectating = true;
    camera2D->SetActive(false);
    cameraFollowShot->SetActive(false);
    pc->SetViewTargetWithBlend(objects->spectatorPawn, 0.0);
}

void APoolCameraManager::InstantCameraMovementForNextShot()
{
    LOG_FUNC
    if (!this)
        ERROR_DEBUG_PRINT_FUNC
    if (!pc->IsLocalPlayerController()) return;
    instantFollow = true;
}

void APoolCameraManager::EndFollowAndSwitchTo2D()
{
    LOG_FUNC
    if (!this)
        ERROR_DEBUG_PRINT_FUNC
    if (!pc->IsLocalPlayerController()) return;
    usingTableCamera = false;
    GetWorldTimerManager().ClearTimer(followTimer);
    cameraFollowShot->SetActive(false);
	/*float InputKey = objects->table->TableViewSpline->FindInputKeyClosestToWorldLocation(player->ArrowComp->GetComponentLocation());
    InputKey = round(InputKey);
    
    FVector pos = objects->table->TableViewSpline->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World);
    if (fabs(pos.X) < 30.0f) pos.X = 0.0f;
    if (fabs(pos.Y) < 30.0f) pos.Y = 0.0f;
    
	//float CurrentPositionTime = InputKey / objects->table->TableViewSpline->GetNumberOfSplinePoints();*/
    //camera2D->SetWorldLocation(pos);
    
    //FRotator NewRot = UKismetMathLibrary::FindLookAtRotation(camera2D->GetComponentLocation(), objects->table->spectatorCameraTarget->GetComponentLocation());
    
    
    
    ResetSpectatorCamera();
    camera2D->SetWorldLocation(objects->spectatorPawn->camera->GetComponentLocation());
    camera2D->SetWorldRotation(objects->spectatorPawn->camera->GetComponentRotation());
    camera2D->SetFieldOfView(objects->spectatorPawn->camera->FieldOfView);
    camera2D->SetActive(true);
    topCamera->SetActive(false);
    lastCameraBlendTime = GetGameTimeSinceCreation();
    if (flowCameraCallbackEngaged)
    {
        flowCameraCallbackEngaged = false;
        objects->gameflow->Aiming(true);
    }
    if (!alreadySwitchedToSpectating)
        pc->SetViewTargetWithBlend(this, instantFollow ? 0.0f : 0.0f);
    instantFollow = false;
}

float APoolCameraManager::SwitchToAiming()
{
    LOG_FUNC
    if (!this)
        ERROR_DEBUG_PRINT_FUNC_RET(0.0f)
    if (!pc->IsLocalPlayerController()) return 0.0f;
    alreadySwitchedToSpectating = false;
   
    player->Activate();
    float blendTime = 0.0f;
    float totalBlendDelay = 0.0f;
    float focusCameraBlendDelay = 0.0f;
    
    if (ShouldAiming)
    {
        blendCamera->SetActorTransform(player->CueCamera->GetComponentTransform());
        //blendTime = from2D ? 0.0f : 0.5f;
        //totalBlendDelay = blendTime + (from2D ? 0.5f : 0.1f);
        cameraSwitchBrokered = false;
        if (objects &&
            objects->gi &&
            objects->gi->data->settings->GetCameraMoveMode() != ECameraMoveMode::FullMode)
        {
            blendTime = 0.0f;
            totalBlendDelay = blendTime + 0.0f;
            FocusCameraToPlayer();
        }
        else
        {
            blendTime = 0.7f;
            totalBlendDelay = blendTime + 0.5f;
            focusCameraBlendDelay = 0.9f;
            
            pc->SetViewTargetWithBlend(blendCamera, blendTime, EViewTargetBlendFunction::VTBlend_Cubic);
            
            if (from2D)
                FocusCameraToPlayer();
            else
                GetWorldTimerManager().SetTimer(DefaultCameraTimer, this, &APoolCameraManager::FocusCameraToPlayer, totalBlendDelay, false);
        }
        
        //from2D = false;
    }
    
    player->ResetCameraViewMode();
    UE_LOG(LogTemp,Error,TEXT("Switch Camera to Aiming"))
    return ShouldAiming ? (from2D ? focusCameraBlendDelay : totalBlendDelay + focusCameraBlendDelay) : 0.0f;
    
}



/*void APoolCameraManager::GeneratePointOnSpline(float TimePosition)
{
    FVector SP_Position = objects->table->TableViewSpline->FindLocationClosestToWorldLocation(player->ArrowComp->GetComponentLocation(),ESplineCoordinateSpace::World);
    objects->spectatorPawn->SetActorLocation(SP_Position);
    FRotator SP_Rotation = UKismetMathLibrary::FindLookAtRotation(objects->spectatorPawn->GetActorLocation(), player->GetActorLocation());
    objects->spectatorPawn->SetActorRotation(player->GetActorRotation());
    FVector SP_Position1 = objects->table->TableViewSpline->GetLocationAtTime(TimePosition += 0.1f,ESplineCoordinateSpace::World);
    FVector SP_Position2 = objects->table->TableViewSpline->GetLocationAtTime(TimePosition -= 0.1f, ESplineCoordinateSpace::World);
    float SP_Distance1 = (SP_Position1 - player->GetActorLocation()).Size();
    float SP_Distance2 = (SP_Position2 - player->GetActorLocation()).Size();
    if (SP_Distance1 > SP_Distance2)
    {
        objects->spectatorPawn->SetActorLocation(SP_Position1);
    }
    else
    {
        objects->spectatorPawn->SetActorLocation(SP_Position2);
    }
    FRotator SP_Rotation;
    SP_Rotation = UKismetMathLibrary::FindLookAtRotation(objects->spectatorPawn->GetActorLocation(), player->GetActorLocation());
    objects->spectatorPawn->SetActorRotation(SP_Rotation);
    
}*/


void APoolCameraManager::FocusCameraToPlayer()
{
    LOG_FUNC
  //  player->CueCamera->Deactivate();
   // player->camera->Activate();
    
    
    float focusCameraBlendDelay = from2D ? 0.0f : 0.9f;
    if (objects &&
        objects->gi &&
        objects->gi->data->settings->GetCameraMoveMode() != ECameraMoveMode::FullMode)
    {
        focusCameraBlendDelay = 0.0f;
    }
    
    if (cameraSwitchBrokered)
    {
        cameraSwitchBrokered = false;
        return;
    }
    pc->SetViewTargetWithBlend(player, focusCameraBlendDelay, EViewTargetBlendFunction::VTBlend_Cubic);
    from2D = false;
}


void APoolCameraManager::ActivateTopCamera()
{
    camera2D->SetActive(false);
    cameraFollowShot->SetActive(false);
    topCamera->SetActive(true);
    Reset2DCam();
    if (pc) pc->SetViewTargetWithBlend(this, 0.0f);
}

void APoolCameraManager::ResetSpectatorCamera()
{
    objects->spectatorPawn->ResetCam();
}

void APoolCameraManager::EngageFlowCallback()
{
    LOG_FUNC
    if (usingTableCamera)
    {
        flowCameraCallbackEngaged = true;
    }
    else if (GetGameTimeSinceCreation() < lastCameraBlendTime + followTime)
    {
        objects->gameflow->Aiming(true);
    }
    else
    {
        objects->gameflow->SubAiming();
    }
}
void APoolCameraManager::SetFrom2D()
{
    from2D = true;
}

void APoolCameraManager::Move2DCam(FVector2D move, float scale)
{
    FVector pos = topCamera->GetComponentLocation();
    pos += FVector(move, 0.0f);
    FVector tableCenter = objects->physicsEngine->GetTableCenter();
    tableCenter.Y += objects->physicsEngine->GetTableSize() / 15.0f;
    
    float tableSize = objects->physicsEngine->GetTableSize();
    const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
    float usedScreenY = ViewportSize.Y * 0.85f;
    const float maxSize = tableSize * 1.4f * ViewportSize.X / (2.0f * usedScreenY);
    
    float newOrthoWidth = topCamera->OrthoWidth - scale;
    newOrthoWidth = FMath::Clamp(newOrthoWidth, tableSize * 0.4f, maxSize);
    topCamera->SetOrthoWidth(newOrthoWidth);
    float effectiveTableSize = maxSize - newOrthoWidth;
    //PRINTF((TEXT("x %f, max X %f"), fabs(pos.X - tableCenter.X), effectiveTableSize / 2.0f), White)
    
    if (fabs(pos.X - tableCenter.X) > effectiveTableSize / 2.0f)
        pos.X = ((pos.X - tableCenter.X) > 0.0f) ?
                tableCenter.X + effectiveTableSize / 2.0f :
                tableCenter.X - effectiveTableSize / 2.0f;
        
    if (fabs(pos.Y - tableCenter.Y) > effectiveTableSize / 4.0f)
        pos.Y = ((pos.Y - tableCenter.Y) > 0.0f) ?
                 tableCenter.Y + effectiveTableSize / 4.0f :
                 tableCenter.Y - effectiveTableSize / 4.0f;
    topCamera->SetWorldLocation(pos);
}

void APoolCameraManager::Reset2DCam()
{
    FVector tableCenter = objects->physicsEngine->GetTableCenter();
    tableCenter.Y += objects->physicsEngine->GetTableSize() / 15.0f;
    FVector pos(tableCenter.X, tableCenter.Y, topCamera->GetComponentLocation().Z);
    topCamera->SetWorldLocation(pos);
    
    float tableSize = objects->physicsEngine->GetTableSize();
    const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
    float usedScreenY = ViewportSize.Y * 0.85f;
    const float maxSize = tableSize * 1.4f * ViewportSize.X / (2.0f * usedScreenY);
    
    topCamera->SetOrthoWidth(maxSize);
}


void APoolCameraManager::BrokerCameraSwitch()
{
    cameraSwitchBrokered = true;
}
