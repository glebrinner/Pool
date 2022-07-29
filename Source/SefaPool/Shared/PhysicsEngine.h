// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolGameStateBase.h"

#include "SharedDefs.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine.generated.h"

class AObjectGatherer;
class ABall;


struct BallPhysicsHelper
{
    ABall* ball;
    UWorld* world;
    AObjectGatherer* objects;
    FVector position;
    FQuat rotation;
    FVector linearVelocity;
    FVector angularVelocity;
    float tableZ;
    bool active;
    bool affected;
    bool isMoving;
    bool evaluateHit;
    float frameTime;
    float movingAverageVelocity;
    bool touchesCloth;
    bool caught;
    int caughtPocketNum;
    bool singleSound;
    
    bool print;
    
    float linearDamping;
    float angularDamping;
    
    BallPhysicsHelper();
    
    UWorld* GetWorld();
    
    float R();
    
    void Populate(ABall* ball_, UWorld* world, AObjectGatherer* objects_, float tableZ);
    
    void Clear();
    
    void CheckHit(float frameTime_);
    
    void ProcessGravity(float dt, FVector& targetPos);
    void ProcessRotation(float dt);
    void ProcessStep(float dt);
    
    bool BallToBandInteraction(FHitResult& hit, float dt, float timeBeforeHit);
    
    void ApplyToWorld();
    
    void ApplyLinearVelocity(FVector vel);
    void ApplyAngularVelocity(FVector vel);
    void PlaceAndStop(FVector pos, FRotator rot);
    
    void Stop();
    void StartMoving();
};

UCLASS()
class SEFAPOOL_API APhysicsEngine : public AActor
{
	GENERATED_BODY()
    AObjectGatherer* objects;
    BallPhysicsHelper ballsInfo[BALL_COUNT_IN_SCENE];
	APoolGameStateBase* GameState;
public:
	uint8 BallCountInGame;
    float ballR;
    float ballRestingOnTableZ;
    FVector2D halfTableCueToDiamond;
	uint8 MovedBallInx;
	// Sets default values for this actor's properties
	APhysicsEngine();

	void InitializePhysicEngine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    
    void PreTick(float frameTime);

public:	
	// Called every frame
	virtual void Tick(float frameTime) override;
    
    void ApplyLinearVelocity(int num, FVector vel);
    void ApplyAngularVelocity(int num, FVector vel);
    bool BallIsMoving(int num);
    void MoveBallAndStop(int num, FVector pos, FRotator rot);
    FVector GetBallLocation(int num);
    FRotator GetBallRotation(int num);
    void ValidateCueBallLinearMovement(FVector target);
    void MoveCueBallIfPositionValid(FVector pos);
    
    FVector2D WorldToTableCoords(FVector pos);
    FVector TableCoordsToWorld(FVector2D pos);
    
    float GetHalfTableLen();
    
    FVector GetTableCenter();

    float GetTableSize();
    
    float GetCushionWidth();
    float GetCushionHeight();
    bool OnTable(FVector2D coords);
    
    bool IsPosFree(FVector2D coords);
    bool IsPosFreeFromCueBall(FVector2D coords);
	void UpdateBallInfo(TArray<ABall*> BallInfoToSet);

	UFUNCTION(BlueprintCallable)
	void SetMovingBallID(int32 BallID) { MovedBallInx = BallID;}\

	uint8 GetMovingBall() { return MovedBallInx;}
};

