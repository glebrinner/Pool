// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "SharedDefs.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Table.h"
#include "UObject/ConstructorHelpers.h"
#include "PoolPlayerControllerBase.h"
#include "Engine/StaticMesh.h"
#include "ObjectGatherer.h"
#include "PoolGameStateBase.h"
#include "Components/SplineComponent.h"
#include "SefaPool/8Ball/BallInfo.h"
#include "Math/UnrealMathUtility.h"

// Sets default values
APoolPlayer::APoolPlayer() :
    root                        (CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"))),
    camera                      (CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"))),
    CueCamera                   (CreateDefaultSubobject<UCameraComponent>(TEXT("CueCamera"))),
    sideCamera                  (CreateDefaultSubobject<UCameraComponent>(TEXT("SideCamera"))),
    fixedHighAngleCamera        (CreateDefaultSubobject<UCameraComponent>(TEXT("FixedHighAngleCamera"))),
    ArrowComp                   (CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"))),
    SpringArmComp               (CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"))),
    sideSpringArmComp           (CreateDefaultSubobject<USpringArmComponent>(TEXT("SideSpringArmComp"))),
    CameraViewStack             (0.15f),
    WalkCameraViewStack         (0.f),
    WalkCameraSpeed             (0.01f),
    CameraViewSpeed             (0.1f),
    cue                         (CreateDefaultSubobject<UCueComponent>(TEXT("CueComp"))),
    hitPoint                    (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HitPointComp"))),
    IsWalkMode                  (false),
    shotPower                   (0.1f),
    aiming                      (false),
    aimPoint                    (FVector2D(0.0f)),
    maxAimDisplacementCoeff     (2.0f),
    maxPower                    (MAX_BALL_LINEAR_VELOCITY),
    minPower                    (MAX_BALL_LINEAR_VELOCITY/20.0f),
    targetBall                  (nullptr),
    targetVec                   (),
    walkDistance                (0.0f),
    walkCameraAdjustment        (),
    aimCameraVerticalAdjustment (),
    effectCloseUpActive         (false),
    cueBall                     (nullptr),
    active                      (false),
    objects                     (nullptr)
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    SetRootComponent(root);
    // Camera settings
    SpringArmComp->bEnableCameraLag = true;
    SpringArmComp->bEnableCameraRotationLag = true;
    SpringArmComp->CameraLagSpeed = 7.f;
    SpringArmComp->TargetArmLength = 0.f;
    SpringArmComp->SetupAttachment(root);
    sideSpringArmComp->SetupAttachment(root);
    
    sideSpringArmComp->bInheritPitch = false;
    sideSpringArmComp->bInheritRoll  = false;
    sideSpringArmComp->bInheritYaw   = false;
    
    camera->SetupAttachment(SpringArmComp);
    
    fixedHighAngleCamera->SetupAttachment(SpringArmComp);
    
    DefaultCameraPos = FVector(-87.65f, 0.0, 7.85f);
    camera->SetRelativeLocation(DefaultCameraPos);
    sideCamera->SetupAttachment(sideSpringArmComp);

    CueCamera->SetupAttachment(SpringArmComp);
    //TODO: fix cues
    cue->SelectCue(1);
    cue->SetWorldScale3D(FVector(1.0f));
    cue->SetRelativeLocation(FVector(-5.0f, 0.0f, 0.0f));
    cue->SetRelativeRotation(FRotator(0.0f, 0.0f, 180.0f));
    cue->SetupAttachment(root);
    cue->SetCollisionProfileName(TEXT("NoCollision"));
    cue->SetSimulatePhysics(false);

    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
    hitPoint->SetStaticMesh(SphereMeshAsset.Object);
    hitPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    hitPoint->SetupAttachment(root);
    hitPoint->SetWorldScale3D(FVector(0.0075f));
    hitPoint->SetCollisionProfileName(TEXT("NoCollision"));
    hitPoint->SetSimulatePhysics(false);

    static ConstructorHelpers::FObjectFinder<UMaterial>HitPointMaterial(TEXT("/Game/ASSETS/Balls/HitPointMaterial.HitPointMaterial"));
    if (HitPointMaterial.Succeeded())
        hitPoint->SetMaterial(0, HitPointMaterial.Object);

    CameraViewSpline = CreateDefaultSubobject<USplineComponent>(TEXT("CameraViewSpline"));
    CameraViewSpline->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    /*CameraViewSpline->SetupAttachment(root);*/
    ArrowComp->SetupAttachment(camera);
    ArrowComp->SetVisibility(false, false);
    /*CameraViewSpline->SetupAttachment(cue);*/
    /*CameraViewSpline->SetupAttachment(RootComponent);*/
    
}

// Called when the game starts or when spawned
void APoolPlayer::BeginPlay()
{
    Super::BeginPlay();
    //PRINTF((TEXT(__FUNCSIG__ " Hello there")), Magenta)
    GATHER_OBJECT_GATHERER(objects)
    cueBall = objects->balls[0];
    if (!cueBall)
    {
        ERROR_DEBUG_PRINT_FUNC_INFO("Cue ball not found")
    }
    else
    {
        ////123 PRINTF((TEXT(__FUNCSIG__ " cueball found")), Red)
        maxAimDisplacementCoeff = 30.0f * cueBall->scale;
    }
    FVector cueBallPos = objects->physicsEngine->GetBallLocation(0);
    root->SetWorldLocation(cueBallPos);
    //SetActorLocation(cueBallPos);
    //FRotator look = UKismetMathLibrary::FindLookAtRotation(cameraPos, cueBallPos);
    SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
    shotPower = 0.0f;// minPower;
    Deactivate();

    FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
    FVector rotationVector = FVector::CrossProduct(cueVector, FVector(0.0f, 0.0f, 1.0f));
    FQuat targetRotation = FQuat(rotationVector, -10.0f/180.0f * 3.141592f) * GetActorRotation().Quaternion();
    SetActorRotation(targetRotation.Rotator());
    cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));

    FVector cameraPos = camera->GetComponentLocation();
    FRotator look = UKismetMathLibrary::FindLookAtRotation(cameraPos, cueBallPos);
    camera->SetWorldRotation(look);
    WalkCameraViewSpline = objects->table->TableViewSpline;
    aimCameraVerticalAdjustment = 0.0f;
    camera->SetActive(true);
    CueCamera->SetActive(false);
    sideCamera->SetActive(false);
    fixedHighAngleCamera->SetActive(false);
    elevated = false;
    fixedYawWhileElevated = 0.0f;
    thisShotHasBeenReset = false;
    //ghostBall->SetAbsolute(true, true, true);
}

// Called to bind functionality to input
void APoolPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APoolPlayer::Activate()
{
    //if (!HasAuthority())
        //PRINTF((TEXT(__FUNCSIG__)), Green)
    UE_LOG(LogTemp, Warning, TEXT("PoolPlayer activating"))
    cueBall = objects->balls[0];
    cue->SetVisibility(true, false);
    hitPoint->SetVisibility(true, false);
    active = true;
    SetActorLocation(objects->physicsEngine->GetBallLocation(0));
    if (!thisShotHasBeenReset)
    {
        FRotator rotation = GetActorRotation();
        rotation.Pitch = rotation.Roll = 0.0f;
        FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
        FVector rotationVector = FVector::CrossProduct(cueVector, FVector(0.0f, 0.0f, 1.0f));
        FQuat targetRotation = FQuat(rotationVector, -10.0f / 180.0f * 3.141592f) * rotation.Quaternion();
        
        FRotator targetRotationRotator(targetRotation);
        
        FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FVector(0.0f));
        targetRotationRotator.Yaw = LookAt.Yaw;
        
        SetActorRotation(targetRotationRotator);
    }
    
    Readjust();
    SwitchElevationCam(false);
    ResetCameraViewMode();

}

void APoolPlayer::Deactivate()
{
    cue->SetVisibility(false, false);
    hitPoint->SetVisibility(false, false);
    objects->playerAimLine->Hide();
    SwitchElevationCam(false);
    active = false;
}

bool APoolPlayer::IsActive()
{
    return active;
}

FVector APoolPlayer::GetCueDisplacement()
{
    const float maxCueDisplacement = 40.0f;
    //float targetCueDisplacement = maxCueDisplacement * (shotPower - minPower) / (maxPower - minPower);
    float targetCueDisplacement = maxCueDisplacement * sqrt(shotPower);

    //FVector verticalRelativeDisp = aimPoint.Y * FVector(0.0f, 0.0f, 1.0f);
    //FVector horizontalAimDisp = aimPoint.X * FVector(0.0f, 1.0f, 0.0f);
    FVector aimDisplacement = FVector(0.0f, aimPoint.X, aimPoint.Y);
    //GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, verticalRelativeDispVec.ToString());
    return -FVector(1.0f, 0.0f, 0.0f) * targetCueDisplacement + aimDisplacement;
}

void APoolPlayer::Readjust()
{
    SetActorLocation(objects->physicsEngine->GetBallLocation(0));
    // ResetCameraViewMode();
    FVector cuePos = cue->GetComponentLocation();
    FVector ballPos = objects->physicsEngine->GetBallLocation(0);
    
    FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
    ////123 PRINTF((TEXT(__FUNCSIG__" cuevector %s"), *cueVector.ToString()), Yellow)
    FVector rotationVector = FVector::CrossProduct(cueVector, FVector(0.0f, 0.0f, 1.0f));
    if (cueVector.Z > 0)
    {
        FQuat targetRotation = FQuat(rotationVector, -UKismetMathLibrary::Atan(cueVector.Z / FVector2D(cueVector.X, cueVector.Y).Size())) * GetActorRotation().Quaternion();
        SetActorRotation(targetRotation.Rotator());
        cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
        cuePos = cue->GetComponentLocation();
    }
    else
    if (-cueVector.Z/FVector2D(cueVector.X, cueVector.Y).Size() > tan(85.0f / 180.0f * 3.141592f))
    {
        FQuat targetRotation = FQuat(rotationVector, 0.1f) * GetActorRotation().Quaternion();
        
        APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*> (GetController());
        
        FRotator rotation = targetRotation.Rotator();
        
        if (pc && pc->IsAngleChanging())
            rotation.Yaw = fixedYawWhileElevated;
        
        SetActorRotation(rotation);
        cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
        cuePos = cue->GetComponentLocation();
    }

    bool clear = false;
    while (!clear)
    {
        FHitResult res = {};
        FCollisionQueryParams params(FName(TEXT("ReadjustingTrace")), true);
        params.bIgnoreTouches = false;
        FCollisionResponseParams params1 = {};
        const float CueLen = 150.0f / GAME_UNIT_IN_CM;
        FVector start = cuePos - cueVector * CueLen;
        FVector end = start + cueVector * (ballPos - start).Size();
        bool found = GetWorld()->LineTraceSingleByChannel(res,
            start,
            end,
            ECC_WorldDynamic,
            params,
            params1);
        if (found && res.Actor != cueBall && (dynamic_cast<ABall*>(res.Actor.Get()) || dynamic_cast<ATable*>(res.Actor.Get())))
        {
            if (-cueVector.Z/FVector2D(cueVector.X, cueVector.Y).Size() < tan(25.0f / 180.0f * 3.141592f))
            {
                FQuat targetRotation = FQuat(rotationVector, -0.01f) * GetActorRotation().Quaternion();
                SetActorRotation(targetRotation.Rotator());
                cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
            }
            else
            if (aimPoint.Y / maxAimDisplacementCoeff < 0.9f)
            {
                FVector2D aimTarget = aimPoint + FVector2D(0.0f, maxAimDisplacementCoeff / 50.0f);
                if (aimTarget.Size() > maxAimDisplacementCoeff)
                {
                    aimTarget.Normalize();
                    aimTarget *= maxAimDisplacementCoeff;
                }
                CHANGE_CUE_POS_NO_READJUST({ aimPoint = aimTarget; })
            }
            /*else
            if (fabs(aimPoint.X) / maxAimDisplacementCoeff > 0.1f)
            {
                FVector2D aimTarget = aimPoint + FVector2D(-aimPoint.X * 0.01f, maxAimDisplacementCoeff / 50.0f);
                if (aimTarget.Size() > maxAimDisplacementCoeff)
                {
                    aimTarget.Normalize();
                    aimTarget *= maxAimDisplacementCoeff;
                }
                CHANGE_CUE_POS_NO_READJUST({ aimPoint = aimTarget; })
            }*/
            else
            {
                FQuat targetRotation = FQuat(rotationVector, -0.01f) * GetActorRotation().Quaternion();
                SetActorRotation(targetRotation.Rotator());
                cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
            }
            
            
            
            cuePos = cue->GetComponentLocation();
        }
        else
        if (found && res.Actor == cueBall && res.Component == cueBall->mesh)
        {
            clear = true;
            hitPoint->SetWorldLocation(res.ImpactPoint);
        }
        else clear = true;
    }
    if (active)
    {
        if (!IsWalkMode)
            objects->playerAimLine->Draw(GetActorLocation(), cueVector, GetActorRotation());
        
        targetBall = objects->playerAimLine->GetTargetBall();
        targetVec = objects->playerAimLine->GetTargetVec();
        if (effectCloseUpActive)
        {
            SetCameraForViewMode(0.0f, 0.0f);
        }
        FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), cueBall->GetActorLocation() + cueVector * aimCameraVerticalAdjustment * MaxAimVerticalAdjustment - FVector(0.0f, 0.0f, 1.0f) * cueVector.Z * 10.0f);
        camera->SetWorldRotation(LookAt);
    }
}

void APoolPlayer::RotateCue(FVector2D rotDelta)
{
    if (!IsWalkMode)
    {
        root->AddRelativeRotation(FRotator(0.f, rotDelta.X, 0.0f));
      /*  camera->AddRelativeRotation(FRotator(rotDelta.Y, 0.f, 0.f));*/
        //SetCameraForViewMode(rotDelta.Y);
        if (fabs(rotDelta.Y) > 1e-5f)
            ChangeCueTableAngle(rotDelta.Y * 10.0f);
        Readjust();
    }
    else 
    {
        SetCameraForWalkViewMode(rotDelta.X);
    }
}

void APoolPlayer::RotateCueTo(FVector to)
{
    if (!IsWalkMode)
    {
        FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), to);
        FRotator rot = root->GetRelativeRotation();
        rot.Yaw = LookAt.Yaw;
        root->SetRelativeRotation(rot);
      /*  camera->AddRelativeRotation(FRotator(rotDelta.Y, 0.f, 0.f));*/
        //SetCameraForViewMode(rotDelta.Y);
        //ChangeCueTableAngle(rotDelta.Y * 10.0f);
        Readjust();
    }
}

void APoolPlayer::SwitchElevationCam(bool enabled)
{
    if (!active) return;
    
    const float CueLen = 150.0f / GAME_UNIT_IN_CM;
    FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
    
    FVector displ_right = FVector::CrossProduct(FVector(0.0f, 0.0f, 1.0f), cueVector);
    displ_right.Normalize();
    if (!enabled)
    {
        if (fabs(cueVector.Z) > 1.0f / 2.0f)
        {
            elevated = true;
            FVector cueBallPos = objects->physicsEngine->GetBallLocation(0);
            FVector deltaPos =  - cueVector * CueLen / 2.0f - displ_right * CueLen / 10.0f;
            FVector cueForward = cueVector;
            cueForward.Z = 0.0f;
            cueForward.Normalize();
            FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(cueBallPos + deltaPos,
                                                                     cueBallPos + cueForward * CueLen / 5.0f);
            
            fixedHighAngleCamera->SetWorldLocation(cueBallPos + deltaPos);
            fixedHighAngleCamera->SetWorldRotation(LookAt);
            camera->SetActive(false);
            sideCamera->SetActive(false);
            CueCamera->SetActive(false);
            fixedHighAngleCamera->SetActive(true);
        }
        else
        {
            elevated = false;
            camera->SetActive(true);
            sideCamera->SetActive(false);
            CueCamera->SetActive(false);
            fixedHighAngleCamera->SetActive(false);
        }
    }
    else
    {
        PRINTF((TEXT("Elevation cam activated")), Green)
        fixedYawWhileElevated = GetActorRotation().Yaw;
        FVector pos = objects->physicsEngine->GetBallLocation(0) + displ_right * CueLen / 2.0f;
        pos += CueLen / 10.0f * FVector(0.0f, 0.0f, 1.0f);
        
        
        FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(pos, pos - displ_right * CueLen / 2.0f);
        
        sideCamera->SetWorldLocation(pos);
        sideCamera->SetWorldRotation(LookAt);
        camera->SetActive(false);
        CueCamera->SetActive(false);
        fixedHighAngleCamera->SetActive(false);
        sideCamera->SetActive(true);
    }
}

void APoolPlayer::CameraAdjust(float adj, float vertical)
{
    if (!IsWalkMode)
    {
        SetCameraForViewMode(adj, vertical);
        Readjust();
    }
    else
    {
        SetCameraForWalkViewMode(adj);
    }
}

void APoolPlayer::MoveAimRelative(FVector2D aimDelta)
{
    MoveAimAbsolute(aimPoint + aimDelta);
}
void APoolPlayer::MoveAimAbsolute(FVector2D aimAbs)
{
    if (aimAbs.Size() > 1.0)
        aimAbs.Normalize();
    aimAbs *= maxAimDisplacementCoeff;
    CHANGE_CUE_POS({ aimPoint = aimAbs; })
}

FVector2D APoolPlayer::GetAbsoluteAimDisplacement()
{
    return aimPoint / maxAimDisplacementCoeff;
}

void APoolPlayer::ChangePowerRelative(float powerDelta)
{
    ChangePowerAbsolute(shotPower + powerDelta);
}
void APoolPlayer::ChangePowerAbsolute(float powerAbs)
{
    if (powerAbs > 1.0f) powerAbs = 1.0f;
    if (powerAbs < 0.0f) powerAbs = 0.0f;
    CHANGE_CUE_POS({ shotPower = powerAbs; })
}

void APoolPlayer::Shoot(APoolPlayerControllerBase* pc)
{
    FVector cuePos = cue->GetComponentLocation();
    FVector ballPos = objects->physicsEngine->GetBallLocation(0);
    FHitResult res = {};
    FCollisionQueryParams params = {};
    FCollisionResponseParams params1 = {};
    FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
    const float CueLen = 150.0f / GAME_UNIT_IN_CM;
    FVector end = cuePos - cueVector * CueLen;
    //FVector end = cuePos + cueVector * (ballPos - cuePos).Size();
    bool found = GetWorld()->LineTraceSingleByChannel(res,
        end,
        cuePos,
        ECC_WorldDynamic,
        params,
        params1);

    if (true || !found || 
        (res.Actor.Get() == cueBall && res.Component.Get() == cueBall->mesh) || 
        (!dynamic_cast<ABall*>(res.Actor.Get()) && !dynamic_cast<ATable*> (res.Actor.Get())))
    {
        float prevShotPower = shotPower;
        FVector2D prevAimPoint = aimPoint;
        //objects->sound->EmitCueHitSound(cuePos, shotPower);
        CHANGE_CUE_POS ({ shotPower = 1 / 2.0f;
                          aimPoint.X = aimPoint.Y = 0.0f; })
        
        if (-cueVector.Z/FVector2D(cueVector.X, cueVector.Y).Size() > tan(30.0f / 180.0f * 3.141592f))
            objects->cameraManager->InstantCameraMovementForNextShot();
        
        pc->InitiateShot(cueVector, prevAimPoint/maxAimDisplacementCoeff, prevShotPower);
        pc->ClientEndPlay(targetBall, targetVec, prevShotPower);
        Deactivate();
        thisShotHasBeenReset = false;
    }
    else
    {
        PRINTF((TEXT("not shooting because of collision: %s"), *(res.Actor->GetName())), Red)
    }
    UE_LOG(LogTemp,Error,TEXT("StartPlay"))
}


void APoolPlayer::ChangeCueTableAngle(float dalpha)
{
    FRotator relativeRot = root->GetRelativeRotation();
    root->AddRelativeRotation(FRotator(FMath::Clamp(relativeRot.Pitch + dalpha, -85.0f, 0.0f) - relativeRot.Pitch, 0.0f, 0.0f));
    Readjust();
}


UCueComponent* APoolPlayer::GetCueComponent()
{
    return cue;
}


/// Camera 

void APoolPlayer::OnTouchEnd(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (!IsWalkMode)
	{
        BPResetCameraFromBorder();
    }
    UE_LOG(LogTemp, Error, TEXT("TouchEnd"))
}

void APoolPlayer::ActivateWalkMode()
{
    /*ResetCameraViewMode();*/
	IsWalkMode = IsWalkMode ? false : true;
    if (IsWalkMode)
    {
        camera->SetActive(true);
        sideCamera->SetActive(false);
        CueCamera->SetActive(false);
        fixedHighAngleCamera->SetActive(false);
        
        objects->playerAimLine->Hide();
        SpringArmComp->bEnableCameraLag = false;
        camera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
        camera->AttachToComponent(root, FAttachmentTransformRules::KeepWorldTransform);
        float InputKey = WalkCameraViewSpline->FindInputKeyClosestToWorldLocation(ArrowComp->GetComponentLocation());
        WalkCameraViewStack = InputKey / WalkCameraViewSpline->GetNumberOfSplinePoints();
    }
    else {
        DeactivateWalkMode();
    }
    //BPBlendCameraToWalkMode(IsWalkMode);
    /*LastValidCameraRotation = camera->GetComponentRotation();*/
//     if (!IsWalkMode)
//     ResetCameraViewMode();
}

void APoolPlayer::DeactivateWalkMode()
{
    /*ResetCameraViewMode();*/
    if (IsWalkMode)
    {
        if (elevated)
        {
            camera->SetActive(false);
            sideCamera->SetActive(false);
            CueCamera->SetActive(false);
            fixedHighAngleCamera->SetActive(true);
        }
        camera->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
        camera->AttachToComponent(SpringArmComp, FAttachmentTransformRules::KeepRelativeTransform);
        SetCameraForViewMode(0.0f, 0.0f);
        IsWalkMode = false;
    }
    /*LastValidCameraRotation = camera->GetComponentRotation();*/
//     if (!IsWalkMode)
//     ResetCameraViewMode();
}



void APoolPlayer::ResetCameraViewMode(bool forced)
{
    if (!IsWalkMode)
    {
        if (thisShotHasBeenReset && !forced) return;
        camera->SetRelativeLocation(DefaultCameraPos);
		float InputKey = CameraViewSpline->FindInputKeyClosestToWorldLocation(camera->GetRelativeLocation());
		CameraViewStack = InputKey / WalkCameraViewSpline->GetNumberOfSplinePoints();
        SetCameraForViewMode(0.0f, -1.0f);
        Readjust();
        thisShotHasBeenReset = true;
        
        //camera->SetRelativeLocation(CameraViewSpline->GetLocationAtSplineInputKey(InputKey, ESplineCoordinateSpace::World));
        /*camera->SetRelativeLocation(CameraViewSpline->GetLocationAtTime(CameraViewStack,ESplineCoordinateSpace::World));*/
        
        //FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), cueBall->GetActorLocation() - GetActorRotation().Pitch / 2.0f * FVector(0.0f, 0.0f, 1.0f));
        //camera->SetWorldRotation(LookAt);
        /*camera->SetWorldRotation(root->GetRelativeRotation());*/
        
    }
    else
    {
        AdjustWalkDist(-walkDistance);
    }
}

void APoolPlayer::SetCameraForViewMode(float Delta, float vertical)
{
    float delta = FMath::Abs(Delta);
    //PRINTF((TEXT("adjusting %f"), vertical), Yellow)
    //if (delta < 0.05f && fabs(vertical) < 0.005f) { return; }
    aimCameraVerticalAdjustment = FMath::Clamp(aimCameraVerticalAdjustment + vertical * CameraViewSpeed, 0.f, 1.f);
    
    CameraViewStack = FMath::Clamp(CameraViewStack += Delta * CameraViewSpeed, 0.f, 1.f);
    
    if (effectCloseUpActive)
    {
        FVector newRelPos = CameraViewSpline->GetLocationAtTime(CameraViewStack, ESplineCoordinateSpace::World, false);
        newRelPos *= 0.8f;
        camera->SetRelativeLocation(newRelPos);
    }
    else
    {
        camera->SetRelativeLocation(CameraViewSpline->GetLocationAtTime(CameraViewStack, ESplineCoordinateSpace::World, false));
    }
    
    FVector loc = camera->GetComponentLocation();
    
    if (effectCloseUpActive)
    {
        FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
        FVector effDisplVector = FVector::CrossProduct(cueVector, FVector(0.0f, 0.0f, 1.0f));
        effDisplVector = FVector::CrossProduct(effDisplVector, cueVector);
        
        const float maxEffectBasedVerticalDisplacement = 3.0f;
        
        APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*> (GetController());
        
        if (pc)
        {
            float verticalEffectDisplSize = maxEffectBasedVerticalDisplacement;
            verticalEffectDisplSize *= 2.0f - pc->GetAimDisplacement().Y;
            
            loc += effDisplVector*verticalEffectDisplSize;
        }
    }
    
    loc += FVector(0.0f, 0.0f, aimCameraVerticalAdjustment * MaxAimVerticalAdjustment);
    
    camera->SetWorldLocation(loc);
    
	/*if (cueBall)
    {
        FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
		FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(camera->GetComponentLocation(), cueBall->GetActorLocation() + cueVector * aimCameraVerticalAdjustment);
		camera->SetWorldRotation(LookAt);
	}*/
}

void APoolPlayer::AdjustWalkCamera(FVector2D adj)
{
    if (!IsWalkMode) return;
    walkCameraAdjustment = adj;
    
    SetCameraForWalkViewMode(0.0f);
}

void APoolPlayer::AdjustWalkDist(float distInc)
{
    if (!IsWalkMode) return;
    walkDistance = FMath::Clamp(walkDistance += distInc, 0.f, 1.f);
    
    SetCameraForWalkViewMode(0.0f);
}

void APoolPlayer::SetCameraForWalkViewMode(float Delta)
{
    WalkCameraViewStack += Delta*WalkCameraSpeed;
    while (WalkCameraViewStack > 1.0f) WalkCameraViewStack -= 1.0f;
    while (WalkCameraViewStack < 0.0f) WalkCameraViewStack += 1.0f;
    FVector loc = WalkCameraViewSpline->GetLocationAtTime(WalkCameraViewStack, ESplineCoordinateSpace::World, false);
    FVector distAdj = loc - objects->physicsEngine->GetTableCenter();
    distAdj.Normalize();
    loc += distAdj * walkDistance * objects->physicsEngine->GetTableSize() / 2.0f;
    
    FVector lookVec = FVector(0.f,0.f,75.f) - loc;
    FVector right = FVector::CrossProduct(lookVec, FVector(0.0f, 0.0f, 1.0f));
    right.Normalize();
    FVector up = FVector::CrossProduct(right, lookVec);
    up.Normalize();
    const float maxShift = 80.0f;
    
    loc += -maxShift * (walkCameraAdjustment.X * right + walkCameraAdjustment.Y * up);
    
    FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(loc, FVector(0.f,0.f,75.f));
    camera->SetWorldLocation(loc);
    camera->SetWorldRotation(LookAt);
}


void APoolPlayer::BallInHandMoveBallIncremental(FVector2D direction)
{
    //thisShotHasBeenReset = false;
    APoolGameStateBase* gs = GetWorld()->GetGameState<APoolGameStateBase>();
    if (!gs) return;
    
    FVector prevBallPos(objects->physicsEngine->GetBallLocation(0));
    
    FVector dirVector = gs->GetFoulBallDefaultReturnLocationOnTable() - gs->GetCueBallDefaultLocationOnTable();
    dirVector.Z = 0;
    dirVector.Normalize();
    FVector orthogonalDir = FVector::CrossProduct(dirVector, FVector(0.0f, 0.0f, 1.0f));
    
    //float speedCoeff = 0.5f;
    
    FVector targetPos = prevBallPos + (-dirVector * direction.Y - orthogonalDir * direction.X);
    
    objects->physicsEngine->ValidateCueBallLinearMovement(targetPos);
    
    /*bool retainLook = objects->playerAimLine->IsGhostVisible();
    if (retainLook)
    {
        FVector ghostPos = objects->playerAimLine->GetGhostPos();
        FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(resultLoc,
                                                                 ghostPos);
        FRotator prevRotation = GetActorRotation();
        const float closestLookSnap = 10.0f * objects->physicsEngine->ballR;
        float coeff = (ghostPos - resultLoc).Size() > closestLookSnap ?
                      1.0f :
                      (ghostPos - resultLoc).Size() / closestLookSnap;
        prevRotation.Yaw = prevRotation.Yaw * (1.0f - coeff) + coeff * LookAt.Yaw;
        SetActorRotation(prevRotation);
    }*/
    
    SetActorLocation(objects->physicsEngine->GetBallLocation(0));
    Readjust();
}


bool APoolPlayer::IsElevated()
{
    return elevated;
}

float APoolPlayer::GetCueAngle()
{
    FVector cueVector = root->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f));
    float angle = atan(-cueVector.Z/FVector2D(cueVector.X, cueVector.Y).Size());
    angle = angle / (3.141592f) * 180.0f;
    return angle;
}


void APoolPlayer::BeginEffectCloseUp()
{
    effectCloseUpActive = true;
    PRINTF((TEXT("BEGINNING close up")), Green)
    Readjust();
}
void APoolPlayer::EndEffectCloseUp()
{
    effectCloseUpActive = false;
    PRINTF((TEXT("ENDING close up")), Green)
    SetCameraForViewMode(0.0f, 0.0f);
    Readjust();
}
