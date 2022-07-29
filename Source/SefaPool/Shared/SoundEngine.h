// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundEngine.generated.h"

UENUM(BlueprintType)
enum class E_PoolSounds : uint8
{
    CUE_HIT = 0   UMETA(DisplayName = "CueHit"),
    BALL_TO_BALL  UMETA(DisplayName = "Ball to ball"),
    DROP          UMETA(DisplayName = "Pocket drop"),
    CUSHION       UMETA(DisplayName = "Cushion"),
};

class AObjectGatherer;

UCLASS()
class SEFAPOOL_API ASoundEngine : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundEngine();

protected:
	virtual void BeginPlay() override;
    
    AObjectGatherer* objects;

public:
    
    UFUNCTION(BlueprintImplementableEvent)
    void EmitCueHitSound(FVector location, float power);
    
    UFUNCTION(BlueprintImplementableEvent)
    void EmitBallToBallHitSound(FVector location, float power);
    
    UFUNCTION(BlueprintImplementableEvent)
    void EmitChalkSound(FVector location);
    
    UFUNCTION(BlueprintImplementableEvent)
    void EmitDropSound(FVector location, float power);
    
    UFUNCTION(BlueprintImplementableEvent)
    void EmitCushionSound(FVector location, float power);
    
    UFUNCTION(BlueprintImplementableEvent)
    void StopSounds();
    
    
    void EmitSound(E_PoolSounds sound, FVector location, float power);
    
    void EmitSound_ClientOrServer(E_PoolSounds sound, FVector location, float power);

};
