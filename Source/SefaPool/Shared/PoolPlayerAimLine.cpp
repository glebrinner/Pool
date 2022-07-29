// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolPlayerAimLine.h"

#include "ObjectGatherer.h"




// Sets default values
APoolPlayerAimLine::APoolPlayerAimLine() :
    currentDiffLevel        (EDifficultyLevels::DIFFICULTY_LEVEL_NONE),
    objects                 (nullptr),
    targetBall              (nullptr),
    targetVec               (),
    root                    (CreateDefaultSubobject<USceneComponent>(TEXT("Root"))),
    ghostBall               (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostBallComp"))),
    shotLine                (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShotLineComp"))),
    afterShotCueLine        (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AfterShotCueLineComp"))),
    afterShotObjectBallLine (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AfterShotObjectLineComp"))),
    ghostPoint              (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostPoint"))),
    triangleAimMaterial     (nullptr),
    squareAimMaterial       (nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    static ConstructorHelpers::FObjectFinder<UStaticMesh>PlaneMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
    UStaticMeshComponent* linesToSetup[] = { shotLine, afterShotCueLine, afterShotObjectBallLine };
    for (auto line : linesToSetup)
    {
        line->SetStaticMesh(PlaneMeshAsset.Object);
        line->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
        line->SetWorldScale3D(FVector(1.0f, 0.0075f, 1.0f));
        line->SetCollisionProfileName(TEXT("NoCollision"));
        line->SetSimulatePhysics(false);
        line->SetupAttachment(root);
    }
    ghostBall->SetStaticMesh(PlaneMeshAsset.Object);
    ghostBall->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    //ghostBall->SetupAttachment(root);
    ghostBall->SetCollisionProfileName(TEXT("NoCollision"));
    ghostBall->SetSimulatePhysics(false);
    ghostBall->SetupAttachment(root);
    static ConstructorHelpers::FObjectFinder<UMaterial> GhostBallMaterial(TEXT("/Game/ASSETS/Balls/GhostMaterial.GhostMaterial"));
    if (GhostBallMaterial.Succeeded())
        ghostBall->SetMaterial(0, GhostBallMaterial.Object);
    static ConstructorHelpers::FObjectFinder<UStaticMesh>SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
    ghostPoint->SetStaticMesh(SphereMeshAsset.Object);
    ghostPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
    ghostPoint->SetCollisionProfileName(TEXT("NoCollision"));
    ghostPoint->SetSimulatePhysics(false);
    ghostPoint->SetupAttachment(root);
    ghostPoint->SetupAttachment(root);
    
    
    static ConstructorHelpers::FObjectFinder<UMaterial> TriangleAimMaterial(TEXT("/Game/ASSETS/InGame/TriangleAimMat.TriangleAimMat"));
    if (TriangleAimMaterial.Succeeded())
        triangleAimMaterial = TriangleAimMaterial.Object;
    
    static ConstructorHelpers::FObjectFinder<UMaterial> TriangleAimGreenMaterial(TEXT("/Game/ASSETS/InGame/TriangleAimGreenMat.TriangleAimGreenMat"));
    if (TriangleAimGreenMaterial.Succeeded())
        triangleAimGreenMaterial = TriangleAimGreenMaterial.Object;
    
    static ConstructorHelpers::FObjectFinder<UMaterial> TriangleAimRedMaterial(TEXT("/Game/ASSETS/InGame/TriangleAimRedMat.TriangleAimRedMat"));
    if (TriangleAimRedMaterial.Succeeded())
        triangleAimRedMaterial = TriangleAimRedMaterial.Object;
    
    
    static ConstructorHelpers::FObjectFinder<UMaterial> GhostMaterial(TEXT("/Game/ASSETS/Balls/GhostMaterial.GhostMaterial"));
    if (GhostMaterial.Succeeded())
    {
        ghostMaterial = GhostMaterial.Object;
        ghostBall->SetMaterial(0, ghostMaterial);
    }
    
    static ConstructorHelpers::FObjectFinder<UMaterial> GhostGreenMaterial(TEXT("/Game/ASSETS/Balls/GhostGreenMaterial.GhostGreenMaterial"));
    if (GhostGreenMaterial.Succeeded())
        ghostGreenMaterial = GhostGreenMaterial.Object;
    
    static ConstructorHelpers::FObjectFinder<UMaterial> GhostRedMaterial(TEXT("/Game/ASSETS/Balls/GhostRedMaterial.GhostRedMaterial"));
    if (GhostRedMaterial.Succeeded())
        ghostRedMaterial = GhostRedMaterial.Object;
    
    
    static ConstructorHelpers::FObjectFinder<UMaterial> SquareAimMaterial(TEXT("/Game/ASSETS/InGame/SquareAimMat.SquareAimMat"));
    if (SquareAimMaterial.Succeeded())
    {
        squareAimMaterial = SquareAimMaterial.Object;
        ghostPoint->SetMaterial(0, squareAimMaterial);
    }
    
    static ConstructorHelpers::FObjectFinder<UMaterial> SquareAimGreenMaterial(TEXT("/Game/ASSETS/InGame/SquareAimGreenMat.SquareAimGreenMat"));
    if (SquareAimGreenMaterial.Succeeded())
        squareAimGreenMaterial = SquareAimGreenMaterial.Object;
    
    static ConstructorHelpers::FObjectFinder<UMaterial> SquareAimRedMaterial(TEXT("/Game/ASSETS/InGame/SquareAimRedMat.SquareAimRedMat"));
    if (SquareAimRedMaterial.Succeeded())
        squareAimRedMaterial = SquareAimRedMaterial.Object;
    
    
}

// Called when the game starts or when spawned
void APoolPlayerAimLine::BeginPlay()
{
	Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects)
    Hide();
    ghostBall->SetWorldScale3D(FVector(objects->balls[0]->scale));
    ghostPoint->SetWorldScale3D(FVector(objects->balls[0]->scale / 5.0f));
    
    //ChangeLevel(EDifficultyLevels::DIFFICULTY_LEVEL_1);
}

void APoolPlayerAimLine::ChangeLevel(EDifficultyLevels level)
{
    currentDiffLevel = level;
    /*switch (currentDiffLevel)
    {
        case EDifficultyLevels::DIFFICULTY_LEVEL_NONE:
        case EDifficultyLevels::DIFFICULTY_LEVEL_6:
        case EDifficultyLevels::DIFFICULTY_LEVEL_5:
            break;
        case EDifficultyLevels::DIFFICULTY_LEVEL_4:
        case EDifficultyLevels::DIFFICULTY_LEVEL_3:
        case EDifficultyLevels::DIFFICULTY_LEVEL_2:
            shotLine->SetWorldScale3D(FVector(1.0f, 0.075f, 1.0f));
            shotLine->SetMaterial(0, triangleAimMaterial);
            afterShotCueLine->SetWorldScale3D(FVector(1.0f, 0.075f, 1.0f));
            afterShotCueLine->SetMaterial(0, triangleAimMaterial);
            afterShotObjectBallLine->SetWorldScale3D(FVector(1.0f, 0.075f, 1.0f));
            afterShotObjectBallLine->SetMaterial(0, triangleAimMaterial);
            break;
    }*/
}

ABall* APoolPlayerAimLine::GetTargetBall()
{
    return targetBall;
}
FVector APoolPlayerAimLine::GetTargetVec()
{
    return targetVec;
}


void APoolPlayerAimLine::Hide()
{
    shotLine->SetVisibility(false, false);
    ghostBall->SetVisibility(false, false);
    ghostPoint->SetVisibility(false, false);
    afterShotObjectBallLine->SetVisibility(false, false);
    afterShotCueLine->SetVisibility(false, false);
}

void APoolPlayerAimLine::Draw(FVector pos, FVector cueVector, FRotator rotation)
{
    SetActorLocation(pos);
    SetActorRotation(rotation);
    
    FVector ballPos = objects->balls[0]->GetActorLocation();
    FVector ghostBallVec = cueVector;
    ghostBallVec.Z = 0.0f;
    ghostBallVec.Normalize();

    FHitResult ghostBallHit;

    FCollisionQueryParams queryParams;
    queryParams.bTraceComplex = true;
    queryParams.AddIgnoredActor(objects->balls[0]);
    queryParams.AddIgnoredActor(objects->opponent);
    
    FCollisionQueryParams queryParamsSecondary;
    queryParamsSecondary.bTraceComplex = true;
    for (int i = 0; i < 16; i++)
        queryParamsSecondary.AddIgnoredActor(objects->balls[i]);
    queryParamsSecondary.AddIgnoredActor(objects->opponent);

    FCollisionResponseParams respParams;
    float ghostBallSphereRadius = objects->physicsEngine->ballR * 0.99f;
    bool hasHit = GetWorld()->SweepSingleByChannel(ghostBallHit,
        ballPos,
        ballPos + ghostBallVec * 200.0f * objects->physicsEngine->ballR,
        FQuat::Identity,
        ECollisionChannel::ECC_Visibility,
        FCollisionShape::MakeSphere(ghostBallSphereRadius),
        queryParams,
        respParams);
    
    UE_LOG(LogTemp, Warning, TEXT("Ghost ball hit %s"), *(ghostBallHit.Actor->GetHumanReadableName()))
    
    
    // ///////////////////////////////////////////
    // ///////////////////////////////////////////
    
    FVector ghostBallLocation;
    FVector shotLineBegin;
    FVector shotLineVector;
    FVector shotLineEnd;
    FRotator shotLineRotation(0.0f);
    
    bool tableHit = false;
    
    float afterShotObjectLineLenCoeff = 0.0f;
    float afterShotCueLineLenCoeff = 0.0f;
    
    bool shouldDrawObjectAfterShot = false;
    
    FRotator objectLineRotation;
    FVector objectLineBegin;
    FVector objectLineVector;
    float objectLineFirstCollisionDistance = 0.0f;
    
    bool shouldDrawCueAfterShot = false;
    
    FRotator cueLineRotation;
    FVector cueLineBegin;
    FVector cueLineVector;
    float cueLineFirstCollisionDistance = 0.0f;
    
    FVector ghostPointLocation;
    
    float secondaryTraceLen = 0.0f;
    
    UMaterial** selectedTriangleMat = &triangleAimMaterial;
    UMaterial** selectedSquareMat = &squareAimMaterial;
    UMaterial** selectedGhostMat = &ghostMaterial;
    
    switch (currentDiffLevel)
    {
        case EDifficultyLevels::DIFFICULTY_LEVEL_NONE:
        case EDifficultyLevels::DIFFICULTY_LEVEL_6:
            secondaryTraceLen = DIFFICULTY_LEVEL_6_OBJ_LEN;
            break;
        case EDifficultyLevels::DIFFICULTY_LEVEL_5:
            secondaryTraceLen = DIFFICULTY_LEVEL_5_OBJ_LEN;
            break;
        case EDifficultyLevels::DIFFICULTY_LEVEL_4:
            secondaryTraceLen = DIFFICULTY_LEVEL_4_OBJ_LEN;
            break;
        case EDifficultyLevels::DIFFICULTY_LEVEL_3:
            secondaryTraceLen = DIFFICULTY_LEVEL_3_OBJ_LEN;
            break;
        case EDifficultyLevels::DIFFICULTY_LEVEL_2:
            secondaryTraceLen = DIFFICULTY_LEVEL_2_OBJ_LEN;
            break;
    }
    secondaryTraceLen /= GAME_UNIT_IN_CM;
    
    // ///////////////////////////////////////////
    // ///////////////////////////////////////////
    
    if (hasHit)
    {
        FVector downVec = (ghostBallSphereRadius - objects->physicsEngine->ballR / 5.0f)*FVector(0.0f, 0.0f, 1.0f);
        
        ghostBallLocation = ghostBallHit.Location - downVec;
        ghostPointLocation = ghostBallHit.ImpactPoint;
        ghostPointLocation.Z = ghostBallHit.Location.Z;
        shotLineVector = ghostBallHit.Location - ballPos;
        shotLineVector.Z = 0.0f;
        
        float shotLenCoeff = shotLineVector.Size() / objects->physicsEngine->halfTableCueToDiamond.Size() / 2.0f;
        float lenInTables = shotLenCoeff;
        shotLenCoeff = -0.02060555 + (1.014484 + 0.02060555)/(1 + pow((shotLenCoeff/0.5435507), 3.474284));
        if (shotLenCoeff > 1.0f)
            shotLenCoeff = 1.0f;
        if (shotLenCoeff < 0.0f)
            shotLenCoeff = 0.0f;
        
        
        shotLineBegin = ballPos - downVec;
        shotLineEnd = shotLineBegin + shotLineVector;
        shotLineVector.Normalize();
        shotLineRotation.Yaw = GetActorRotation().Yaw;

        ABall* hitTarget = dynamic_cast<ABall*>(ghostBallHit.Actor.Get());
        if (hitTarget)
        {
            int targetNum = hitTarget->ballNum;
            
            APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
            if (pc && pc->PlayerState)
            {
                APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (pc->PlayerState);
                if (ps)
                {
                    if (ps->CanTargetThisBall(targetNum))
                    {
                        selectedTriangleMat = &triangleAimGreenMaterial;
                        selectedSquareMat = &squareAimGreenMaterial;
                        selectedGhostMat = &ghostGreenMaterial;
                    }
                    else
                    {
                        selectedTriangleMat = &triangleAimRedMaterial;
                        selectedSquareMat = &squareAimRedMaterial;
                        selectedGhostMat = &ghostRedMaterial;
                    }
                        
                }
            }
            
            targetBall = hitTarget;
            FVector centersVector = hitTarget->GetActorLocation() - ghostBallHit.Location;
            centersVector.Z = 0.0f;
            centersVector.Normalize();
            targetVec = centersVector;
            float hitAngleCos = FVector::DotProduct(ghostBallVec, centersVector);
            FVector hitAngleSin = FVector::CrossProduct(ghostBallVec, centersVector);
            float incidenceAngleInDegrees = acosf(hitAngleCos) / PI * 180.0f * (hitAngleSin.Z > 0.0f ? 1.0f : -1.0f);
            afterShotObjectLineLenCoeff = hitAngleCos * shotLenCoeff;
            afterShotCueLineLenCoeff = hitAngleSin.Size() * shotLenCoeff;
            
            if (afterShotObjectLineLenCoeff > 1e-2)
            {
                //afterShotObjectLineLenCoeff = 1.0f;
                shouldDrawObjectAfterShot = true;
                FVector objectLineStart = hitTarget->GetActorLocation() - downVec;
                objectLineBegin = objectLineStart;
                objectLineVector = centersVector;
                
                //FVector objectLineEnd = objectLineStart + centersVector * lenBase * afterShotObjectLineLenCoeff;
                FRotator prevRotationObject = afterShotObjectBallLine->GetComponentRotation();
                prevRotationObject.Pitch = 0.0f;
                prevRotationObject.Roll = 0.0f;
                prevRotationObject.Yaw = rotation.Yaw + incidenceAngleInDegrees;
                objectLineRotation = prevRotationObject;
                
                
                FHitResult objectBallHit;

                FCollisionResponseParams respParamsObject;
                FVector objectTraceBegin = objectLineBegin + FVector(0.0f, 0.0f, objects->physicsEngine->ballR);
                objectLineFirstCollisionDistance = afterShotObjectLineLenCoeff * secondaryTraceLen;
                bool hasHitObject = GetWorld()->SweepSingleByChannel(objectBallHit,
                                                               objectTraceBegin,
                                                               objectTraceBegin + objectLineVector * objectLineFirstCollisionDistance,
                                                                FQuat::Identity,
                                                                ECollisionChannel::ECC_Visibility,
                                                                FCollisionShape::MakeSphere(ghostBallSphereRadius / 10.0f),
                                                                     queryParamsSecondary,
                                                               respParamsObject);
                if (hasHitObject)
                {
                    objectLineFirstCollisionDistance = fmin((objectBallHit.Location - objectTraceBegin).Size() + objects->physicsEngine->ballR, objectLineFirstCollisionDistance);
                }
                
            }
            else
                shouldDrawObjectAfterShot = false;
            if (afterShotCueLineLenCoeff > 1e-2)
            {
                //afterShotCueLineLenCoeff = 0.5f;
                afterShotCueLineLenCoeff /= 2.0f;
                shouldDrawCueAfterShot = true;
                FVector cueLineStart = shotLineEnd;
                cueLineBegin = cueLineStart;
                FVector cueAfterShotVector = -FVector::CrossProduct(hitAngleSin, centersVector);
                cueAfterShotVector.Normalize();
                cueLineVector = cueAfterShotVector;
                FRotator prevRotationCue = afterShotCueLine->GetComponentRotation();
                prevRotationCue.Pitch = 0.0f;
                prevRotationCue.Roll = 0.0f;
                prevRotationCue.Yaw = rotation.Yaw + incidenceAngleInDegrees - 90.0f;
                if (hitAngleSin.Z < 0.0f ) prevRotationCue.Yaw += 180.0f;
                cueLineRotation = prevRotationCue;
                
                
                FHitResult cueBallHit;

                FCollisionResponseParams respParamsCue;
                FVector cueTraceBegin = objectLineBegin + FVector(0.0f, 0.0f, objects->physicsEngine->ballR);
                cueLineFirstCollisionDistance = afterShotCueLineLenCoeff * secondaryTraceLen;
                bool hasHitCue = GetWorld()->SweepSingleByChannel(cueBallHit,
                                                                  cueTraceBegin,
                                                                  cueTraceBegin + cueLineVector * cueLineFirstCollisionDistance,
                                                                FQuat::Identity,
                                                                ECollisionChannel::ECC_Visibility,
                                                                FCollisionShape::MakeSphere(ghostBallSphereRadius / 10.0f),
                                                                  queryParamsSecondary,
                                                                  respParamsCue);
                if (hasHitCue)
                {
                    cueLineFirstCollisionDistance = fmin((cueBallHit.Location - cueTraceBegin).Size() + objects->physicsEngine->ballR, cueLineFirstCollisionDistance);
                }
                
            }
            else
                shouldDrawCueAfterShot = false;
        }
        else
        {
            /* HIT TABLE */
            tableHit = true;
            targetBall = nullptr;
            afterShotObjectBallLine->SetVisibility(false, false);
            afterShotCueLine->SetVisibility(false, false);
            shouldDrawObjectAfterShot = false;
            shouldDrawCueAfterShot = false;
        }

    }
    
    if (!hasHit)
    {
        Hide();
        return;
    }
    
    const float shotLineXScaleFactor = (shotLineEnd - shotLineBegin).Size();
    const float shotLineLenInCm = shotLineXScaleFactor / GAME_UNIT_IN_CM;
    shotLine->SetWorldRotation(shotLineRotation);
    
    if (currentDiffLevel != EDifficultyLevels::DIFFICULTY_LEVEL_1)
    {
        if (shouldDrawObjectAfterShot)
        {
            afterShotObjectBallLine->SetWorldLocation(objectLineBegin +
                                                      objectLineVector * objectLineFirstCollisionDistance / 2.0f);
            switch (currentDiffLevel)
            {
                case EDifficultyLevels::DIFFICULTY_LEVEL_NONE:
                case EDifficultyLevels::DIFFICULTY_LEVEL_6:
                case EDifficultyLevels::DIFFICULTY_LEVEL_5:
                    afterShotObjectBallLine->SetMaterial(0, *selectedSquareMat);
                    afterShotObjectBallLine->SetWorldScale3D(FVector(objectLineFirstCollisionDistance / 100.0f, 0.0075f, 1.0f));
                    break;
                case EDifficultyLevels::DIFFICULTY_LEVEL_4:
                case EDifficultyLevels::DIFFICULTY_LEVEL_3:
                case EDifficultyLevels::DIFFICULTY_LEVEL_2:
                    afterShotObjectBallLine->SetMaterial(0, *selectedTriangleMat);
                    afterShotObjectBallLine->SetWorldScale3D(FVector(objectLineFirstCollisionDistance / 100.0f, 0.075f, 1.0f));
                    break;
            }
            afterShotObjectBallLine->SetWorldRotation(objectLineRotation);
            afterShotObjectBallLine->SetVisibility(true, false);
        }
        else
            afterShotObjectBallLine->SetVisibility(false, false);
        
        if (shouldDrawCueAfterShot)
        {
            afterShotCueLine->SetWorldLocation(cueLineBegin + cueLineVector * cueLineFirstCollisionDistance / 2.0f);
            afterShotCueLine->SetWorldRotation(cueLineRotation);
            switch (currentDiffLevel)
            {
                case EDifficultyLevels::DIFFICULTY_LEVEL_NONE:
                case EDifficultyLevels::DIFFICULTY_LEVEL_6:
                case EDifficultyLevels::DIFFICULTY_LEVEL_5:
                    afterShotCueLine->SetMaterial(0, *selectedSquareMat);
                    afterShotCueLine->SetWorldScale3D(FVector(cueLineFirstCollisionDistance / 100.0f, 0.0075f, 1.0f));
                    break;
                case EDifficultyLevels::DIFFICULTY_LEVEL_4:
                case EDifficultyLevels::DIFFICULTY_LEVEL_3:
                case EDifficultyLevels::DIFFICULTY_LEVEL_2:
                    afterShotCueLine->SetMaterial(0, *selectedTriangleMat);
                    afterShotCueLine->SetWorldScale3D(FVector(cueLineFirstCollisionDistance / 100.0f, 0.075f, 1.0f));
                    break;
            }
            afterShotCueLine->SetVisibility(true, false);
        }
        else
            afterShotCueLine->SetVisibility(false, false);
    }
    
    float thresholdLen234 = 0.0f;
    switch (currentDiffLevel)
    {
        case EDifficultyLevels::DIFFICULTY_LEVEL_4:
            thresholdLen234 = DIFFICULTY_LEVEL_4_SHOT_LEN;
            break;
        case EDifficultyLevels::DIFFICULTY_LEVEL_3:
            thresholdLen234 = DIFFICULTY_LEVEL_3_SHOT_LEN;
            break;
        case EDifficultyLevels::DIFFICULTY_LEVEL_2:
            thresholdLen234 = DIFFICULTY_LEVEL_2_SHOT_LEN;
            break;
    }
    
    switch (currentDiffLevel)
    {
        case EDifficultyLevels::DIFFICULTY_LEVEL_NONE:
        case EDifficultyLevels::DIFFICULTY_LEVEL_6:
        case EDifficultyLevels::DIFFICULTY_LEVEL_5:
        {
            shotLine->SetWorldLocation((shotLineBegin + shotLineEnd) / 2.0f);
            shotLine->SetWorldScale3D(FVector(shotLineXScaleFactor / 100.0f, 0.0075f, 1.0f));
            shotLine->SetMaterial(0, squareAimMaterial);
            shotLine->SetVisibility(true, false);
        
            ghostBall->SetWorldLocation(ghostBallLocation);
            ghostBall->SetWorldRotation(FRotator(0.0f));
            ghostBall->SetVisibility(true, false);
            ghostBall->SetMaterial(0, *selectedGhostMat);
            ghostPoint->SetVisibility(false, false);
            break;
        }
        case EDifficultyLevels::DIFFICULTY_LEVEL_4:
        case EDifficultyLevels::DIFFICULTY_LEVEL_3:
        case EDifficultyLevels::DIFFICULTY_LEVEL_2:
        {
            if (shotLineLenInCm > thresholdLen234 || tableHit)
            {
                float len = fmin(shotLineLenInCm, thresholdLen234) * GAME_UNIT_IN_CM;
                shotLine->SetWorldScale3D(FVector(len / 100.0f, 0.075f, 1.0f));
                shotLine->SetWorldLocation(shotLineBegin + shotLineVector * len / 2.0f);
                shotLine->SetMaterial(0, triangleAimMaterial);
                ghostBall->SetVisibility(false, false);
                afterShotObjectBallLine->SetVisibility(false, false);
                afterShotCueLine->SetVisibility(false, false);
            }
            else
            {
                shotLine->SetWorldLocation((shotLineBegin + shotLineEnd) / 2.0f);
                //shotLine->SetWorldScale3D(FVector(shotLineXScaleFactor / 100.0f, 0.0075f, 1.0f));
                shotLine->SetWorldScale3D(FVector(shotLineXScaleFactor / 100.0f, 0.075f, 1.0f));
                shotLine->SetMaterial(0, triangleAimMaterial);
                //shotLine->SetMaterial(0, squareAimMaterial);
                shotLine->SetVisibility(true, false);
                
                ghostBall->SetWorldLocation(ghostBallLocation);
                ghostBall->SetVisibility(true, false);
                ghostBall->SetMaterial(0, *selectedGhostMat);
            }
            shotLine->SetVisibility(true, false);
            //ghostPoint->SetVisibility(false, false);
            break;
        }
        case EDifficultyLevels::DIFFICULTY_LEVEL_1:
        {
            ghostPoint->SetWorldLocation(ghostPointLocation);
            ghostPoint->SetMaterial(0, *selectedSquareMat);
            shotLine->SetVisibility(false, false);
            ghostBall->SetVisibility(false, false);
            afterShotObjectBallLine->SetVisibility(false, false);
            afterShotCueLine->SetVisibility(false, false);
            ghostPoint->SetVisibility(true, false);
            break;
        }
    }
    
}

void APoolPlayerAimLine::ForceUpdateTargetableBalls()
{
    APlayerController* PC = UGameplayStatics::GetPlayerControllerFromID(GetWorld(),0);
    if (PC)
    {
        APoolPlayer* PoolPlayer = Cast<APoolPlayer>(PC->GetPawn());
        if (PoolPlayer)
        {
            Draw(PoolPlayer->GetActorLocation(),PoolPlayer->GetRoot()->GetComponentTransform().TransformVectorNoScale(FVector(1.0f, 0.0f, 0.0f)),PoolPlayer->GetActorRotation());
        }else UE_LOG(LogTemp,Error,TEXT("Pool player is not valid "))
    }else UE_LOG(LogTemp,Error,TEXT("Player controller is not valid for force update targetable balls"))
}


bool APoolPlayerAimLine::IsGhostVisible()
{
    return ghostBall->IsVisible();
}
FVector APoolPlayerAimLine::GetGhostPos()
{
    return IsGhostVisible() ? ghostBall->GetComponentLocation() : FVector();
}
