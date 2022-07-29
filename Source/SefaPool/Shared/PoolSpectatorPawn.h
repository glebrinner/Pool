// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "PoolSpectatorPawn.generated.h"

class AObjectGatherer;

UCLASS()
class SEFAPOOL_API APoolSpectatorPawn : public APawn
{
	GENERATED_BODY()
    AObjectGatherer* objects;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* root;
public:
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* camera;
private:
    UPROPERTY(EditAnywhere)
    float defaultSplineLoc;

public:
	// Sets default values for this pawn's properties
	APoolSpectatorPawn();

	void ResetCam();
    void AdjustLook(FVector loc);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
