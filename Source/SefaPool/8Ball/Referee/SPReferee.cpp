 // Fill out your copyright notice in the Description page of Project Settings.


#include "SPReferee.h"

#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/SinglePlayer/PoolSPGameMode.h"
#include "SefaPool/SinglePlayer/PoolSPPlayerController.h"

class APoolGameStateBase;


 void ASPReferee::BeginPlay()
 {
	 Super::BeginPlay();
 	firstShot = true;
 	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
 	if (gs)
 	{
 		gs->SetRuleset(ShotRuleset::RULES_BEHIND_HEADSTRING);
 	}
 	
 }

void ASPReferee::NextShot(uint8 pc_num)
{
    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
    if (gs)
    {
        if (firstShot)
            gs->SetRuleset(RULES_BEHIND_HEADSTRING);
        else
            gs->SetRuleset(RULES_ANYWHERE);
    }
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->ballInHand = true;
        ps->UpdateClient();
        UpdateTargetableBalls(ps);
    }
    UE_LOG(LogTemp,Error,TEXT("NextShot"))
}

void ASPReferee::ProcessBallPocketed(int num, bool first)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        uint16 message = MESSAGE_POCKETED_CUE_BALL + num;
        ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
    }
    
    
	PRINTF((TEXT("Ball %d has been pocketed"), num), Blue)
    if (num == 0)
    {
    	PRINTF((TEXT("Returning cue ball")), Blue)
        gs->ReturnCueBall();
    }
    else
    {
    	// already taken care of
    	//gs->StorePocketedBall(num);
    }
}

void ASPReferee::ProcessBallKnockedOff(int num)
{
	// cue => issue foul/loss
	// other => return to point
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        uint16 message = MESSAGE_KNOCKED_OFF_CUE_BALL + num;
        ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
    }
    
	if (num == 0)
	{
		gs->ReturnCueBall();
	}
	else
	{
		gs->ReturnFoulBall(num);
	}
}

void ASPReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
	// UE_LOG(LogTemp,Error,TEXT("Current Idle Time %f"),CurrentIdleTime)
    Super::ProcessHitLogAndFinalize(hitLog);
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->Server_DeployMessages();
    }
    
 	firstShot=false;
	bool inGame = false;
 	APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
 	GS->SetRuleset();
	for (int i = 1; i < 16; i++)
	{
		if (GS->ballsInGame[i])
		{
			inGame = true;
			break;
		}
	}
	APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
	if (!inGame)
	{
		SPplayingPc->ClientTravelToLevel("Authentication", "WinScreen");
	}
	
	else
	{
		APoolSPGameMode* GM = Cast<APoolSPGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
			GM->NextShot(SPplayingPc);
	}
}
void ASPReferee::UpdateTargetableBalls(APoolPlayerStateBase* PlayerState)
{
	TArray<uint8> TargetableBalls;
	for (uint8 i = 0; i < BallCountInGame; ++i)
	{
		TargetableBalls.Add(i);
	}
	if (PlayerState)
	{
		PlayerState->ClientSetTargetableBalls(TargetableBalls);
	}
}
