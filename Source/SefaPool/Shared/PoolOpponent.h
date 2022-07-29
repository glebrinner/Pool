// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ball.h"
#include "CueComponent.h"
#include "PoolOpponent.generated.h"

class AObjectGatherer;

UCLASS()
class SEFAPOOL_API APoolOpponent : public AActor
{
	GENERATED_BODY()
    
    
    UPROPERTY(VisibleAnywhere)
    USceneComponent* root;
public:
    UPROPERTY(VisibleAnywhere)
    UCueComponent* cue;
    
private:
    
    AObjectGatherer* objects;
    ABall* cueBall;
    FVector targetPosition;
    FRotator targetRotation;
    FVector targetRelativeCuePosition;
    FRotator targetRelativeCueRotation;
    
    FVector prevPosition;
    FRotator prevRotation;
    FVector prevRelativeCuePosition;
    FRotator prevRelativeCueRotation;
    
    float lastMessageTime;
    float catchUpTime;
    bool following;
    
public:	
	// Sets default values for this actor's properties
	APoolOpponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    
    void EnableAndUpdatePosition(int cueNum, FVector position,
                                 FRotator rotation,
                                 FVector relativeCuePosition,
                                 FRotator relativeCueRotation);
    void Disable();

};
