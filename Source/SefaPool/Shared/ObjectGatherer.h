// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Ball.h"
#include "Table.h"
#include "TablePockets.h"
#include "TableWalls.h"
#include "BallRestLocation.h"
#include "PhysicsEngine.h"
#include "PoolCameraManager.h"
#include "PoolTableCameras.h"
#include "PoolSpectatorPawn.h"
#include "SharedDefs.h"
#include "SefaPool/8Ball/BallInfo.h"
#include "SefaPool/8Ball/BallsVisualization.h"
#include "SefaPoolGameInstance.h"
#include "SoundEngine.h"
#include "PoolGameFlow.h"
#include "PoolHUD.h"
#include "ShotInfo.h"
#include "UserInputValidation.h"
#include "PoolPlayerAimLine.h"
#include "PoolOpponent.h"



#include "ObjectGatherer.generated.h"

UCLASS()
class SEFAPOOL_API AObjectGatherer : public AActor
{
	GENERATED_BODY()
    bool gathered;
public:
    class ABallManagerBase* BallManager;
    ABall* balls[BALL_COUNT_IN_SCENE];
    ATable* table;
    ATablePockets* tablePockets;
    ATableWalls* tableWalls;
    ABallRestLocation* ballRestLocation;
    APhysicsEngine* physicsEngine;
    APoolCameraManager* cameraManager;
    APoolTableCameras* tableCameras;
    APoolSpectatorPawn* spectatorPawn;
    APhysicsSetup* settings;
    ASoundEngine* sound;
    APoolGameFlow* gameflow;
    APoolHUD* hud;
	ABallInfo* ballinfo;
	ABallsVisualization* ballvisualization;
    AShotInfo* shotinfo;
    AUserInputValidation* validation;
    APoolPlayerAimLine* playerAimLine;
    APoolOpponent* opponent;
    USefaPoolGameInstance* gi;
	class AReplayManager* ReplayManager;
	class AReplayVisualization* ReplayVizualization;
    
	// Sets default values for this actor's properties
	AObjectGatherer();
    
    void operator () ();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable)
	ABall* GetBall(int32 Inx);
};

#define GATHER_OBJECT_GATHERER(ptr) \
if (!ptr) {\
TArray<AActor*> foundObjectGathererActor; \
UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObjectGatherer::StaticClass(), foundObjectGathererActor); \
if (foundObjectGathererActor.Num() != 1) \
{ \
    ERROR_DEBUG_PRINT_FUNC_INFO(__FUNCSIG__ ":: ObjectGatherer not found") \
} \
else \
{ \
    ptr = dynamic_cast<AObjectGatherer*> (foundObjectGathererActor[0]);\
    (*ptr) (); \
} }
