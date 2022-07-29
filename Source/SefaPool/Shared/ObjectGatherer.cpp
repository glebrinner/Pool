// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectGatherer.h"
#include "SefaPool/8Ball/BallInfo.h"
#include "SefaPool/8Ball/BallsVisualization.h"
#include "SefaPool/8Ball/BallsGatherer/BallManagerBase.h"
#include "ReplaySystem/ReplayManager.h"
#include "ReplaySystem/ReplayVisualization.h"
#include "SefaPool/Shared/ReplaySystem/ReplayVisualization.h"

// Sets default values
AObjectGatherer::AObjectGatherer()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AObjectGatherer::BeginPlay()
{
	Super::BeginPlay();
    gathered = false;
}

ABall* AObjectGatherer::GetBall(int32 Inx)
{
    return balls[Inx];
}

void AObjectGatherer::operator () ()
{
    if (gathered) return;
    gathered = true;
    TArray<AActor*> FoundActors;
    
#define GATHER(error, type, num, callback) \
UGameplayStatics::GetAllActorsOfClass(GetWorld(), type::StaticClass(), FoundActors); \
if (FoundActors.Num() != num) \
{ \
    if (error)\
        ERROR_DEBUG_PRINT_FUNC_INFO("ObjectGatherer:: " #type " not found or broken") \
    else callback\
} \
else \
{\
    callback\
}
#define DEF_CALLBACK(type, object) \
    object = dynamic_cast<type*>(FoundActors[0]);
    
#define GATHER_SINGLE(type, var) \
    GATHER(true, type, 1, DEF_CALLBACK(type, var))
    
    GATHER_SINGLE(ATable,               table)
    GATHER_SINGLE(ATablePockets,        tablePockets)
    GATHER_SINGLE(ATableWalls,          tableWalls)
    GATHER_SINGLE(ABallRestLocation,    ballRestLocation)
    GATHER_SINGLE(APhysicsEngine,       physicsEngine)
    GATHER_SINGLE(APoolCameraManager,   cameraManager)
    GATHER_SINGLE(APoolTableCameras,    tableCameras)
    GATHER_SINGLE(APoolSpectatorPawn,   spectatorPawn)
    GATHER_SINGLE(APhysicsSetup,        settings)
    GATHER_SINGLE(ASoundEngine,         sound)
    GATHER_SINGLE(APoolGameFlow,        gameflow)
    GATHER_SINGLE(ABallInfo,            ballinfo)
    GATHER_SINGLE(ABallsVisualization,  ballvisualization)
    GATHER_SINGLE(AShotInfo,            shotinfo)
    GATHER_SINGLE(AUserInputValidation, validation)
    GATHER_SINGLE(APoolPlayerAimLine,   playerAimLine)
    GATHER_SINGLE(APoolOpponent,        opponent)
    GATHER_SINGLE(ABallManagerBase, BallManager)
    GATHER_SINGLE(AReplayManager, ReplayManager)
    GATHER_SINGLE(AReplayVisualization, ReplayVizualization)
    
    PRINTF((TEXT("GATHERING BALLS")), White)
    GATHER(true, ABall, BALL_COUNT_IN_SCENE, {
        for (int i = 0; i < BALL_COUNT_IN_SCENE; i++)
        {
            const auto ballNum = dynamic_cast<ABall*>(FoundActors[i])->ballNum;
            balls[ballNum] = dynamic_cast<ABall*>(FoundActors[i]);
            //PRINTF((TEXT("found ball %d"), ballNum), White)
        }
    })
    PRINTF((TEXT("GATHERED BALLS")), White)
    GATHER(false, APoolHUD, 1, {
        if (FoundActors.Num() == 0) hud = nullptr;
        else
        {
            hud = dynamic_cast<APoolHUD*> (FoundActors[0]);
        }
    })
    
    gi = Cast<USefaPoolGameInstance>(GetGameInstance());
    if (!gi)
        ERROR_DEBUG_PRINT_FUNC_INFO(__FUNCSIG__ ":: GI not found")
    
#undef GATHER
#undef DEF_CALLBACK
}
