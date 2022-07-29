// Fill out your copyright notice in the Description page of Project Settings.


#include "PhysicsSetup.h"
#include "PoolGameStateBase.h"

// Sets default values
APhysicsSetup::APhysicsSetup():
	startDecelerationVelocity (5.0),
	stopVelocity              (1.0),
	defaultAngularDamping     (0.5),
	slowingAngularDamping     (0.7),
	defaultLinearDamping      (0.5),
	slowingLinearDamping      (0.7),
	impulsePropagationDelay   (0.2),
    orthoCamMovementTableFraction (20.0f),
	normalVelocityLoss		  (0.9),
	xTouchSensitivity		  (0.05),
	yTouchSensitivity		  (0.01),
	xAxisOrientation		  (1),
	yAxisOrientation		  (1),
    defaultSplineLoc          (0.6f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APhysicsSetup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APhysicsSetup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    //GAME_STATE_SYNC(gs, CustomPhysicsIteration(DeltaTime))
}

