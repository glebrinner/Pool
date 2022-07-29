// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolOpponent.h"

#include "ObjectGatherer.h"

// Sets default values
APoolOpponent::APoolOpponent() :
    root        (CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"))),
    cue         (CreateDefaultSubobject<UCueComponent>(TEXT("CueComp"))),
    objects     (nullptr),
    cueBall     (nullptr),
    targetPosition (),
    targetRotation (),
    targetRelativeCuePosition (),
    targetRelativeCueRotation (),
    prevPosition (),
    prevRotation (),
    prevRelativeCuePosition (),
    prevRelativeCueRotation (),
    lastMessageTime (0.0f),
    catchUpTime (1.0f),
    following (false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    cue->SetupAttachment(root);
    cue->SetVisibility(false, false);
}

// Called when the game starts or when spawned
void APoolOpponent::BeginPlay()
{
	Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects)
    cueBall = objects->balls[0];
	
}

// Called every frame
void APoolOpponent::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    if (!following)
        return;
    
    float mixCoeff = (GetGameTimeSinceCreation() - lastMessageTime) / catchUpTime;
    FVector newPosition;
    FRotator newRotation;
    FVector newRelativeCuePosition;
    FRotator newRelativeCueRotation;
    if (mixCoeff > 1.0f)
    {
        newPosition = targetPosition;
        newRotation = targetRotation;
        newRelativeCuePosition = targetRelativeCuePosition;
        newRelativeCueRotation = targetRelativeCueRotation;
    }
    else
    {
        newPosition = FMath::Lerp(prevPosition,
                                  targetPosition,
                                  mixCoeff);
        newRotation = FQuat::Slerp(prevRotation.Quaternion(),
                                    targetRotation.Quaternion(),
                                    mixCoeff).Rotator();
        newRelativeCuePosition = FMath::Lerp(prevRelativeCuePosition,
                                             targetRelativeCuePosition,
                                             mixCoeff);
        newRelativeCueRotation = FQuat::Slerp(prevRelativeCueRotation.Quaternion(),
                                               targetRelativeCueRotation.Quaternion(),
                                               mixCoeff).Rotator();
    }
    
    objects->physicsEngine->MoveBallAndStop(0, newPosition, FRotator(0.0f));
    SetActorLocation(newPosition);
    SetActorRotation(newRotation);
    cue->SetRelativeLocation(newRelativeCuePosition);
    cue->SetRelativeRotation(newRelativeCueRotation);
}

void APoolOpponent::EnableAndUpdatePosition(int cueNum,
                                            FVector position,
                                            FRotator rotation,
                                            FVector relativeCuePosition,
                                            FRotator relativeCueRotation)
{
    cue->SetVisibility(true, false);
    cue->SelectCue(cueNum);
    lastMessageTime = GetGameTimeSinceCreation();
    
    targetPosition = position;
    targetRotation = rotation;
    targetRelativeCuePosition = relativeCuePosition;
    targetRelativeCueRotation = relativeCueRotation;
    
    prevPosition = GetActorLocation();
    prevRotation = GetActorRotation();
    prevRelativeCuePosition = cue->GetRelativeLocation();
    prevRelativeCueRotation = cue->GetRelativeRotation();
    
    if (!following)
    {
        prevPosition = targetPosition;
        prevRotation = targetRotation;
        prevRelativeCuePosition = targetRelativeCuePosition;
        prevRelativeCueRotation = targetRelativeCueRotation;
        
        SetActorLocation(prevPosition);
        SetActorRotation(prevRotation);
        cue->SetRelativeLocation(prevRelativeCuePosition);
        cue->SetRelativeRotation(prevRelativeCueRotation);
        objects->physicsEngine->MoveBallAndStop(0, prevPosition, FRotator(0.0f));
    }
    
    following = true;
}
void APoolOpponent::Disable()
{
    following = false;
    cue->SetVisibility(false, false);
}



