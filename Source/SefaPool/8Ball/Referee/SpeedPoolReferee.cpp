// Fill out your copyright notice in the Description page of Project Settings.


#include "SpeedPoolReferee.h"

#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"
#include "SefaPool/SinglePlayer/PoolSPGameMode.h"
#include "SefaPool/SinglePlayer/PoolSPPlayerController.h"

void ASpeedPoolReferee::BeginPlay()
{
	Super::BeginPlay();
    prevCueBallFoulCount = CueBallFoulCount = 0;
	objects->ReplayManager->InitializeGameMode(ERecordedGameMode::SpeedPool);
	
}


void ASpeedPoolReferee::ProcessBallPocketed(int num, bool first)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
    if (ps && num)
    {
        uint16 message = MESSAGE_POCKETED_CUE_BALL + num;
        ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
    }
	if (!num)
	{
		CueBallFoulCount++;
		TotalTimeInGame += PenaltyTime;
        
        if (ps) ps->RegisterMessage(E_MessageSeverity::SECONDARY, MESSAGE_SPEED_POOL_PENALTY_SCRATCH);
        
		UE_LOG(LogTemp,Error,TEXT("Penalty event called"))
		
        //if (CueBallFoulCount < 3)
        gs->ReturnCueBall();
	}
    else
    {
        CountOfPocketedBalls++;
    }
}

void ASpeedPoolReferee::ProcessBallKnockedOff(int num)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
    PRINTF((TEXT("Ball %d has been pocketed"), num), Blue)
    if (num == 0)
    {
        CueBallFoulCount++;
        TotalTimeInGame += PenaltyTime;
    
        if (ps) ps->RegisterMessage(E_MessageSeverity::SECONDARY, MESSAGE_SPEED_POOL_PENALTY_KNOCKED_OFF);
        
        //if (CueBallFoulCount < 3)
        gs->ReturnCueBall();
    }
    else
    {
        if (ps)
        {
            uint16 message = MESSAGE_KNOCKED_OFF_CUE_BALL + num;
            ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
        }
        gs->ReturnFoulBall(num);
    }
}

void ASpeedPoolReferee::OnPlayerStageStart()
{
    Super::OnPlayerStageStart();
	
}

void ASpeedPoolReferee::OnPlayerStageEnd()
{
    Super::OnPlayerStageEnd();
}

void ASpeedPoolReferee::OnPlayTimeEnd()
{
    CueBallFoulCount++;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps) ps->RegisterMessage(E_MessageSeverity::SECONDARY, MESSAGE_SPEED_POOL_PENALTY_CUE_TIMER);
    Super::OnPlayTimeEnd();
    
}


void ASpeedPoolReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
    Super::ProcessHitLogAndFinalize(hitLog);
	if (GetLocalRole() < ROLE_Authority) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->Server_DeployMessages();
    }
    
    if (prevCueBallFoulCount == CueBallFoulCount)
    {
        prevCueBallFoulCount = CueBallFoulCount = 0;
        ps->ballInHand = false;
    }
    else
    {
        prevCueBallFoulCount = CueBallFoulCount;
        ps->ballInHand = true;
    }
    ps->UpdateClient();
    
    if (CueBallFoulCount >= 3)
    {
        ps->SendImmediateMessage(E_MessageSeverity::PRIMARY, MESSAGE_SPEED_POOL_CONSECUTIVE_FOULS);
        
        APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
        //GetCurrentTotalInGameTime();
        SPplayingPc->ClientTravelToLevel("Authentication", "LoseScreen");
        GameResults();
        return;
    }
    
    
	// UE_LOG(LogTemp,Error,TEXT("Current Idle Time %f"),CurrentIdleTime)
	bool inGame = false;
	for (int i = 1; i < 16; i++)
	{
		if (dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState())->ballsInGame[i])
		{
			inGame = true;
			break;
		}
	}
	APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
	if (!inGame)
	{
		TotalTimeInGame+= GetGameTimeSinceCreation() - TotalIdleTime;
		SPplayingPc->ClientTravelToLevel("Authentication", "WinScreen");
        GameResults();
		UE_LOG(LogTemp,Error,TEXT("Idle time increase"))
	}
	
	else
	{
		APoolSPGameMode* GM = Cast<APoolSPGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->NextShot(SPplayingPc);
	}
	
}

int ASpeedPoolReferee::ContestId()
{
    return static_cast<int> (EContestType::CONTEST_SPEED_POOL);
}

