// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolPlayer.h"
#include "MainSpectatorPawn.generated.h"

UCLASS()
class SEFAPOOL_API AMainSpectatorPawn : public APoolPlayer
{
	GENERATED_BODY()

public:
	AMainSpectatorPawn();

protected:
	// class AObjectGatherer* ObjectGatherer;

	virtual void BeginPlay() override;

	// void Init();

public:	
	// UFUNCTION(BlueprintCallable)
	// void InitiateSwitch2D();
	//
	// UFUNCTION(BlueprintCallable)
	// void InitiateSwitch3D();
 //    
	// UFUNCTION(BlueprintCallable)
	// void RegisterJoystickMovement(FVector2D newPos);

	void Readjust() override;
	
	// virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//
	// void SetCameraTo2D();

	
	
};
