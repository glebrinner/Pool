// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Table.h"
#include "Ball.h"
#include "TablePockets.h"
#include "PoolTableCameras.generated.h"
class AObjectGetherer;
UCLASS()
class SEFAPOOL_API APoolTableCameras : public AActor
{
	GENERATED_BODY()
    UPROPERTY(VisibleAnywhere)
	TArray<FVector2D> pocketCenters;
	UPROPERTY(VisibleAnywhere)
	TArray<UCameraComponent*> pocketCameras;
	ATable* table;
	float tableLen;
    
    AObjectGatherer* objects;
public:	
	// Sets default values for this actor's properties
	APoolTableCameras();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	bool ProcessShot(ABall* targetBall, FVector targetVector);
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
