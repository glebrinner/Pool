// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#include "Math/Vector.h"
#include "Components/SceneComponent.h"
#include "PhysicsEngine.h"
#include "TablePockets.generated.h"

class AObjectGatherer;

UCLASS()
class SEFAPOOL_API ATablePockets : public AActor
{
	GENERATED_BODY()
    UPROPERTY(VisibleAnywhere)
    USceneComponent* root;
public:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* pocket_0;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* pocket_1;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* pocket_2;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* pocket_3;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* pocket_4;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* pocket_5;
	
    UStaticMeshComponent* pocket_ptrs[6];
    AObjectGatherer* objects;
    float clothZ;
    FVector tableCenter;
    
public:	
	// Sets default values for this actor's properties
	ATablePockets();
    
    void SetBaseZLevel(float z);
    bool IntersectingPocket(FVector pos);
    
    void EvaluateCaughtPostStep(BallPhysicsHelper* ballPtr);
    
    void GetZLevelForBallAndCheckIfPocketed(BallPhysicsHelper* ballPtr, float& z, bool& pocketed, FVector& laterAcc);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

};
