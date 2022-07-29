// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Ball.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CueComponent.h"
#include "PoolPlayer.generated.h"


class AObjectGatherer;
class APoolPlayerControllerBase;
class USplineComponent;
class USpringArmComponent;

UCLASS()
class SEFAPOOL_API APoolPlayer : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USceneComponent* root;
public:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UCameraComponent* camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* CueCamera;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* sideCamera;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UCameraComponent* fixedHighAngleCamera;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UArrowComponent* ArrowComp;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	USpringArmComponent* SpringArmComp;
	
    UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
    USpringArmComponent* sideSpringArmComp;
    
	UPROPERTY(BlueprintReadOnly)
		float CameraViewStack;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
		float WalkCameraViewStack;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	 float WalkCameraSpeed;

	UPROPERTY(EditAnywhere)
		float CameraViewSpeed;
    
	void ResetCameraViewMode(bool forced = false);

	void OnTouchEnd(ETouchIndex::Type FingerIndex, FVector Location);

	UFUNCTION(BlueprintCallable)
	void ActivateWalkMode();
    UFUNCTION(BlueprintCallable)
    void DeactivateWalkMode();

	void SetCameraForViewMode(float Delta, float vertical);
    void AdjustWalkCamera(FVector2D adj);
    void AdjustWalkDist(float distInc);
    
	void SetCameraForWalkViewMode(float Delta);

	UFUNCTION(BlueprintImplementableEvent)
	void BPResetCameraFromBorder();

	UFUNCTION(BlueprintImplementableEvent)
	void BPBlendCameraToWalkMode(bool IsActive);

	UFUNCTION(BlueprintImplementableEvent)
	void BPTest();

	

protected:
	UPROPERTY(VisibleAnywhere)
    UCueComponent* cue;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* hitPoint;

	bool IsWalkMode;
	float shotPower;
	bool aiming;
	FVector2D aimPoint;
	float maxAimDisplacementCoeff;
	float maxPower;
	float minPower;
	ABall* targetBall;
    FVector targetVec;
    bool thisShotHasBeenReset;
    
    const float MaxAimVerticalAdjustment = 50.0f;
    
    float walkDistance;

    FVector2D walkCameraAdjustment;
    
    float aimCameraVerticalAdjustment;
    
    bool effectCloseUpActive;
public:
	UPROPERTY(BlueprintReadOnly);
	ABall* cueBall;
private:
	bool active;
    AObjectGatherer* objects;
public:
	// Sets default values for this pawn's properties
	APoolPlayer();

protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
		FVector DefaultCameraPos;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
		USplineComponent* CameraViewSpline;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USplineComponent* WalkCameraViewSpline;
    
    bool elevated;
    float fixedYawWhileElevated;

	FRotator LastValidCameraRotation;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
public:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Activate();
	virtual void Deactivate();
	bool IsActive();
	bool IsWalkModeActivate() { return IsWalkMode; }

	FVector GetCueDisplacement();
	virtual void Readjust();


#define CHANGE_CUE_POS_NO_READJUST(action) \
{FVector oldDisp = GetCueDisplacement(); \
action \
FVector newDisp = GetCueDisplacement(); \
FVector loc = cue->GetRelativeLocation(); \
loc += newDisp - oldDisp; \
cue->SetRelativeLocation(loc);}

#define CHANGE_CUE_POS(action) \
{CHANGE_CUE_POS_NO_READJUST(action) \
Readjust();}

	void RotateCue(FVector2D rotDelta);
    
    void RotateCueTo(FVector to);
    
    void SwitchElevationCam(bool enabled);
    
	void MoveAimRelative(FVector2D aimDelta);
	void MoveAimAbsolute(FVector2D aimAbs);
	void ChangePowerRelative(float powerDelta);
	void ChangePowerAbsolute(float powerAbs);
	void Shoot(APoolPlayerControllerBase* pc);
    
    
    void ChangeCueTableAngle(float dalpha);
    void CameraAdjust(float adj, float vertical);
	FVector2D GetAbsoluteAimDisplacement();
    
    UCueComponent* GetCueComponent();
	UFUNCTION(BlueprintPure)
	USceneComponent* GetRoot() { return root; }
    
    
    void BallInHandMoveBallIncremental(FVector2D vec);
    
    bool IsElevated();
    
    float GetCueAngle();
    
    void BeginEffectCloseUp();
    void EndEffectCloseUp();

};
