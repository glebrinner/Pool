// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "PoolTableCameras.h"
#include "Camera/CameraActor.h"
#include "PoolCameraManager.generated.h"

class APoolPlayerControllerBase;
class APoolPlayer;
class APoolSpectatorPawn;
class ATable;
class ABall;
class AObjectGatherer;
class UCameraComponent;

UCLASS()
class SEFAPOOL_API APoolCameraManager : public AActor
{
    GENERATED_BODY()
    UPROPERTY(VisibleAnywhere)
    USceneComponent* root;
public:
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
    UCameraComponent* camera2D;
    UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
    UCameraComponent* topCamera;
private:
    UPROPERTY(VisibleAnywhere)
    UCameraComponent* cameraFollowShot;
    AObjectGatherer* objects;
    ACameraActor* blendCamera;

    APoolPlayer* player;
    ABall* targetBall;
public:
    APoolPlayerControllerBase* pc;
private:
    float followTime;
    FTimerHandle followTimer;
    FTimerHandle DefaultCameraTimer;
    FVector followTargetPos;
    bool usingTableCamera;
    bool flowCameraCallbackEngaged; // delays the switch to aiming if too fast after table -> 2D camera
    float lastCameraBlendTime;
    bool alreadySwitchedToSpectating;
    bool from2D;
    bool cameraSwitchBrokered;
    bool instantFollow;
public:
    // Sets default values for this actor's properties
    APoolCameraManager();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    
   // float temp;


    UFUNCTION(BlueprintImplementableEvent)
        void BP_FocusCameraToBall(AActor* Ball);

        void FocusCameraOnBall();

  
    void EndFollowAndSwitchTo2D();
    
    //void GeneratePointOnSpline(float TimePosition);

public:
    bool ShouldAiming;

    virtual void Tick(float DeltaTime) override;
    bool IsCameraLocked();
    
    void EngageFlowCallback();
    
    void FocusCameraToPlayer();

    void Populate(APoolPlayerControllerBase* pc_,
                  APoolPlayer* player_);
    void StartFollowing(ABall* targetBall = nullptr, FVector targetVector = FVector(0.0f), float shotPower = 0.0f);
    void SwitchToSpectating();
    
    void InstantCameraMovementForNextShot();
    
    float SwitchToAiming();
    
    void ActivateTopCamera();
    
    void ResetSpectatorCamera();
    
    void SetFrom2D();
    
    void Move2DCam(FVector2D move, float scale);
    void Reset2DCam();
    
    void BrokerCameraSwitch();
    
};
