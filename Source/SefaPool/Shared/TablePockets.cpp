// Fill out your copyright notice in the Description page of Project Settings.


#include "TablePockets.h"
#include "ObjectGatherer.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATablePockets::ATablePockets() :
    root        (nullptr),
    pocket_0    (nullptr),
    pocket_1    (nullptr),
    pocket_2    (nullptr),
    pocket_3    (nullptr),
    pocket_4    (nullptr),
    pocket_5    (nullptr),
    pocket_ptrs (),
    objects     (nullptr),
    clothZ      (0.0f),
    tableCenter ()
{
	PrimaryActorTick.bCanEverTick = false;
    
    root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = root;
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Game/ASSETS/Shapes/Shape_Cylinder.Shape_Cylinder"));
#define POCKET(n, name, scale, loc) \
    pocket_##n = CreateDefaultSubobject<UStaticMeshComponent>(TEXT(name)); \
    pocket_##n->SetStaticMesh(CylinderAsset.Object);\
    pocket_##n->SetRelativeLocation(loc);\
    pocket_##n->SetCollisionProfileName(TEXT("OverlapAll"));\
    pocket_##n->SetWorldScale3D(scale);\
    pocket_##n->SetVisibility(false, false);\
    pocket_ptrs[n] = pocket_##n;

    POCKET(0, "pocket_0_mesh", FVector(0.1, 0.1, 0.01), FVector(0.0f))
    POCKET(1, "pocket_1_mesh", FVector(0.1, 0.1, 0.01), FVector(0.0f))
    POCKET(2, "pocket_2_mesh", FVector(0.1, 0.1, 0.01), FVector(0.0f))
    POCKET(3, "pocket_3_mesh", FVector(0.1, 0.1, 0.01), FVector(0.0f))
    POCKET(4, "pocket_4_mesh", FVector(0.08, 0.08, 0.01), FVector(0.0f))
    POCKET(5, "pocket_5_mesh", FVector(0.08, 0.08, 0.01), FVector(0.0f))
#undef POCKET
}

// Called when the game starts or when spawned
void ATablePockets::BeginPlay()
{
	Super::BeginPlay();
    
    GATHER_OBJECT_GATHERER(objects)
    tableCenter = (objects->table->GetCueBallDefaultPos() + objects->table->GetFoulDefaultPos()) / 2.0f;
    tableCenter.Z = 0.0f;
}

void ATablePockets::SetBaseZLevel(float z)
{
    clothZ = z;
}

bool ATablePockets::IntersectingPocket(FVector pos)
{
    const float ballR = objects->physicsEngine->ballR;
    for (int i = 0; i < 6; i ++)
    {
        FVector pocketLocation = pocket_ptrs[i]->GetComponentLocation();
        float pocketR = pocket_ptrs[i]->GetComponentScale().X * 50.0f;
        FVector xy_vector = pos - pocketLocation;
        xy_vector.Z = 0.0f;
        
        if (xy_vector.Size() <= pocketR * 1.2f)
        {
            return true;
        }
    }
    return false;
}

void ATablePockets::EvaluateCaughtPostStep(BallPhysicsHelper* ballPtr)
{
    if (ballPtr->caught)
    {
        const float ballR = ballPtr->R();
        const int pocketNum = ballPtr->caughtPocketNum;
        FVector pocketLocation = pocket_ptrs[pocketNum]->GetComponentLocation();
        float pocketR = pocket_ptrs[pocketNum]->GetComponentScale().X * 50.0f;
        FVector xy_vector = ballPtr->position - pocketLocation;
        xy_vector.Z = 0.0f;
        
        
        if (xy_vector.Size() > pocketR - ballR)
        {
            float saved_z = ballPtr->position.Z;
            FVector normalVec = xy_vector;
            normalVec.Normalize();
            ballPtr->position = pocketLocation + normalVec * (pocketR - ballR);
            ballPtr->position.Z = saved_z;
            FVector normalVel = ballPtr->linearVelocity.ProjectOnToNormal(normalVec);
            
            const float insidePocketNormalSpeedLoss = 0.5f;
            const float insidePocketTangentSpeedLoss = 0.5f;
            float saved_vel_z = ballPtr->linearVelocity.Z;
            ballPtr->linearVelocity.Z = 0.0f;
            ballPtr->linearVelocity -= normalVel;
            ballPtr->linearVelocity *= (1.0f - insidePocketTangentSpeedLoss);
            ballPtr->linearVelocity -= normalVel * (1.0f - insidePocketNormalSpeedLoss);
            ballPtr->linearVelocity.Z = saved_vel_z;
        }
    }
}

void ATablePockets::GetZLevelForBallAndCheckIfPocketed(BallPhysicsHelper* ballPtr,
                                                       float& z,
                                                       bool& pocketed,
                                                       FVector& laterAcc)
{
    const float ballR = ballPtr->R();
    
    if (ballPtr->position.Z > objects->physicsEngine->GetCushionHeight() + z)
    {
        z = clothZ + ballPtr->R();
        laterAcc = FVector(0.0f);
        pocketed = false;
        return;
    }
    //ballPtr->caught = false;
    
    for (int i = 0; i < 6; i ++)
    {
        FVector pocketLocation = pocket_ptrs[i]->GetComponentLocation();
        float pocketR = pocket_ptrs[i]->GetComponentScale().X * 50.0f;
        FVector xy_vector = ballPtr->position - pocketLocation;
        xy_vector.Z = 0.0f;
        FVector xy_toTableCenter = tableCenter - pocketLocation;
        xy_toTableCenter.Z = 0.0f;
        
        float dotProduct = FVector::DotProduct(xy_vector, xy_toTableCenter);
        
        if (dotProduct < 0.0f || xy_vector.Size() <= pocketR - ballR)
        {
            laterAcc = FVector(0.0f);
            if (ballPtr->position.Z < pocketLocation.Z)
            {
                pocketed = true;
                z = NAN;
                ballPtr->caught = true;
                ballPtr->caughtPocketNum = i;
                return;
            }
            else
            {
                z = pocketLocation.Z - ballR;
                pocketed = false;
                ballPtr->caught = true;
                ballPtr->caughtPocketNum = i;
                return;
            }
        }
        else if (xy_vector.Size() > pocketR) continue;
        else if (xy_vector.Size() > pocketR - ballR)
        {
            z = clothZ + sqrt(ballR * ballR - (pocketR - xy_vector.Size())*(pocketR - xy_vector.Size()));
            laterAcc = -xy_vector;
            laterAcc.Normalize();
            laterAcc *= (pocketR - xy_vector.Size())/ballR;
            pocketed = false;
            return;
        }
    }
    z = clothZ + ballPtr->R();
    laterAcc = FVector(0.0f);
    pocketed = false;
}

