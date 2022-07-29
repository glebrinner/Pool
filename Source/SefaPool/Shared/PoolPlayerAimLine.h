// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine.h"
#include "Ball.h"
#include "PoolPlayerAimLine.generated.h"

UENUM(BlueprintType)
enum class EDifficultyLevels : uint8
{
    DIFFICULTY_LEVEL_NONE = 0 UMETA(DisplayName = "None"),
    DIFFICULTY_LEVEL_1    = 1 UMETA(DisplayName = "1"),
    DIFFICULTY_LEVEL_2    = 2 UMETA(DisplayName = "2"),
    DIFFICULTY_LEVEL_3    = 3 UMETA(DisplayName = "3"),
    DIFFICULTY_LEVEL_4    = 4 UMETA(DisplayName = "4"),
    DIFFICULTY_LEVEL_5    = 5 UMETA(DisplayName = "5"),
    DIFFICULTY_LEVEL_6    = 6 UMETA(DisplayName = "6"),
};


const float DIFFICULTY_LEVEL_4_SHOT_LEN = 150.0f,
            DIFFICULTY_LEVEL_3_SHOT_LEN = 100.0f,
            DIFFICULTY_LEVEL_2_SHOT_LEN = 50.0f,
            DIFFICULTY_LEVEL_6_OBJ_LEN  = 45.0f,
            DIFFICULTY_LEVEL_5_OBJ_LEN  = 40.0f,
            DIFFICULTY_LEVEL_4_OBJ_LEN  = 35.0f,
            DIFFICULTY_LEVEL_3_OBJ_LEN  = 25.0f,
            DIFFICULTY_LEVEL_2_OBJ_LEN  = 15.0f;

class AObjectGatherer;

UCLASS()
class SEFAPOOL_API APoolPlayerAimLine : public AActor
{
	GENERATED_BODY()
    EDifficultyLevels currentDiffLevel;
    
    AObjectGatherer* objects;
    
    ABall* targetBall;
    FVector targetVec;
public:
    
	APoolPlayerAimLine();
    
    UPROPERTY(VisibleAnywhere)
    USceneComponent* root;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ghostBall;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* shotLine;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* afterShotCueLine;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* afterShotObjectBallLine;
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ghostPoint;
    
    UMaterial* triangleAimMaterial;
    UMaterial* triangleAimGreenMaterial;
    UMaterial* triangleAimRedMaterial;
    
    UMaterial* squareAimMaterial;
    UMaterial* squareAimGreenMaterial;
    UMaterial* squareAimRedMaterial;
    
    UMaterial* ghostMaterial;
    UMaterial* ghostGreenMaterial;
    UMaterial* ghostRedMaterial;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
    
    void Draw(FVector pos, FVector cueVector, FRotator rotation);
	void ForceUpdateTargetableBalls();
    void Hide();
    UFUNCTION(BlueprintCallable)
    void ChangeLevel(EDifficultyLevels level);
    
    ABall* GetTargetBall();
    FVector GetTargetVec();
    
    bool IsGhostVisible();
    FVector GetGhostPos();
};
