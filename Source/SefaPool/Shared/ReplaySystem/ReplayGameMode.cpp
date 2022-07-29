// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayGameMode.h"

#include "ReplayManager.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/PoolPlayerControllerBase.h"
#include "SefaPool/Shared/PoolPlayerStateBase.h"

AReplayGameMode::AReplayGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	PlayerControllerClass = APoolPlayerControllerBase::StaticClass();
	PlayerStateClass = APoolPlayerStateBase::StaticClass();
	GameStateClass = APoolGameStateBase::StaticClass();
	
}

void AReplayGameMode::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(ObjectGatherer);
	ObjectGatherer->ballinfo->SetIsReplayMode(true);
}

void AReplayGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PlayerPC = Cast<APoolPlayerControllerBase>(NewPlayer);
	if (!PlayerPC)
	{
		UE_LOG(LogTemp,Warning,TEXT("PlayerPC not valid"))
	}
}

void AReplayGameMode::InitializeReplayManager()
{
    UE_LOG(LogTemp, Error, TEXT("Loaading match replay"))
	ObjectGatherer->ReplayManager->LoadMatchReplay();
	ObjectGatherer->ReplayManager->PrepareToPlayReplay(PlayerPC);
}


void AReplayGameMode::PCReady(APoolPlayerControllerBase* pc)
{
    if (!this || !pc)
        ERROR_DEBUG_PRINT_FUNC
        
    pc->ClientSetViewToTable();
    
    PlayerPC = pc;
    
    if (levelReady)
    {
        PRINTF((TEXT("PCReady:: next shot")), Red)
        StartGame();
    }
    else
    {
        PRINTF((TEXT("PCReady:: no next shot")), Red)
    }
}

void AReplayGameMode::StartGame()
{
    InitializeReplayManager();
    // ObjectGatherer->ReplayManager->PlayReplay();
}

void AReplayGameMode::LevelReady()
{
    levelReady = true;
    PRINTF((TEXT("LevelReady")), Green)
    if (PlayerPC)
    {
        PRINTF((TEXT("LevelReady:: next shot")), Green)
        StartGame();
    }
    else
        PRINTF((TEXT("no next shot")), Red)
}

