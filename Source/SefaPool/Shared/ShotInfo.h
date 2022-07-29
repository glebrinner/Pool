// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RenderCore.h"
#include "ShotInfo.generated.h"

const int SIDERAIL_HIT = 117;
class AObjectGatherer;

USTRUCT()
struct FHitInfo
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY()
    int obj1;
    UPROPERTY()
    int obj2;
    UPROPERTY()
    FVector2D pos;
    UPROPERTY()
    float time;
};

UCLASS()
class SEFAPOOL_API AShotInfo : public AActor
{
	GENERATED_BODY()
    
    AObjectGatherer* objects;
    
    bool ballsActiveDuringShot[22];
    TArray<FHitInfo> hitLog;
    bool newPocketedBalls[22];
    bool newKnockedBalls[22];
    bool networked;
public:
    bool inShot;
private:
    int firstPocketed;
    float shotStartTime;
    
    FTimer sendToClientsTimer;
    
    bool shotEnded;
    
public:
    
    bool* ballsInGame;
protected:
	virtual void BeginPlay() override;

public:
    AShotInfo();
    virtual void Tick(float dt);
    
    void SetNetworked(bool val);
    
    bool IsInShot() { return inShot; }
    void AddToHitLog(int obj1, int obj2, FVector pos, float time);
    void BallPocketed(int num);
    void BallKnockedOff(int num);
    void BallStopped(int num);
    void BallMoved(int num);
    void ShotEnded();
    void ResetForNextShot();
    
    float GetTimeSinceShotBegan();
    

};
