// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainSpectatorPawn.h"
#include "GameFramework/Actor.h"
#include "PoolGameFlow.generated.h"

enum class GameFlowState_t
{
    INIT          = 1,
    SPECTATING,
    AIMING,
    AIMING_HAND,
    BALL_IN_HAND,
    AFTER_SHOT,
    CAMERA_2D_AIMING,
    CAMERA_2D_SPECTATING,
    CAMERA_3D_AIMING,
    CAMERA_3D_SPECTATING
};


class AObjectGatherer;
class ABall;
class APoolPlayer;

UCLASS()
class SEFAPOOL_API APoolGameFlow : public AActor
{
	GENERATED_BODY()
    AObjectGatherer* objects;
	
    
public:
    GameFlowState_t state;
    APoolPlayer* pawn;
	AMainSpectatorPawn* SpecPawn;
	// Sets default values for this actor's properties
	APoolGameFlow();
    void SetPawn(APoolPlayer* pawn_);
	void SetSpecPawn(AMainSpectatorPawn* SpecPawn_);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    
    bool ballInHand;
    bool grabbed2D;
    bool changingAngle;
    

public:	
	// Called every frame
    
    void Spectating();
    void SubAiming();
    void SubAimingForced();
    void Aiming(bool timed);
    void SubSubAiming();
    void ActuallyStartAimingAfterCameraEffect();
    void BallInHand();
    void SwitchBallInHand();
    void AfterShot(ABall* targetBall, FVector targetVec, float shotPower);
    void Switch2D();
    void Camera2DAiming();
    void Camera2DSpectating(bool justCamera = false);
    void Switch3D();
    void Stop3D();
    void Camera3DAiming();
    void Camera3DSpectating();
    
    void SetBallInHand(bool val);
    void SetChangingAngle(bool val);
    bool GetChangingAngle();

};
