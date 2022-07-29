// Fill out your copyright notice in the Description page of Project Settings.


#include "MPRefereeBase.h"

#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ShotInfo.h"
#include "SefaPool/8Ball/Pool8BallPlayerController.h"

AMPRefereeBase::AMPRefereeBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsTeamMode = false;
	FirstTeamCurrentPlayerNum = 0;
	SecondTeamCurrentPlayerNum = 0;
}

void AMPRefereeBase::BeginPlay()
{
	Super::BeginPlay();
	shotInfo.Clear();
}

void AMPRefereeBase::OnGameStart()
{
	Super::OnGameStart();
	shotInfo.openTable = true;
	if (bIsTeamMode)
	{
		FirstTeam = {playingPcs[0],playingPcs[2]};
		SecondTeam = {playingPcs[1],playingPcs[3]};
	}
}


void AMPRefereeBase::OnPlayTimeEnd()
{
    playingPcs[shotInfo.currentPlayerNum]->ShotTimeHasEnded();
    shotInfo.shotResult = E_ShotResult::FOUL;
	CheckRules_Finalize();
}

void AMPRefereeBase::ProcessBallPocketed(int num, bool first)
{
    /*APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
    APool8BallPlayerState* ps = Cast<APool8BallPlayerState>(pc->PlayerState);
    APool8BallPlayerController* other_pc = Cast<APool8BallPlayerController>(playingPcs[1 - shotInfo.currentPlayerNum]);
    APool8BallPlayerState* other_ps = Cast<APool8BallPlayerState>(other_pc->PlayerState);
    
    if (ps)
    {
        uint16 message = MESSAGE_KNOCKED_OFF_CUE_BALL + num;
        ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
    }*/
    
    
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
	if (num == 0)
	{
		shotInfo.cuePocketed = true;
		gs->ReturnCueBall();
	}
	else
	{
		if (first) shotInfo.firstPocketedBall = num;
		gs->ballsInGame[num] = false;
		gs->StorePocketedBall(num);
        
		if (num == 8) shotInfo.blackPocketed = true;
		else
			if (num < 8) shotInfo.lowPocketed = true;
			else
				if (num > 8) shotInfo.highPocketed = true;
	}
}

void AMPRefereeBase::ProcessBallKnockedOff(int num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
	if (num == 0)
	{
		gs->ReturnCueBall();
		shotInfo.cueKnocked = true;
	}
	else
	{
		gs->ReturnFoulBall(num);
	}
}

void AMPRefereeBase::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = Cast<APool8BallPlayerState>(pc->PlayerState);
	APool8BallPlayerController* other_pc = Cast<APool8BallPlayerController>(playingPcs[1 - shotInfo.currentPlayerNum]);
	APool8BallPlayerState* other_ps = Cast<APool8BallPlayerState>(other_pc->PlayerState);
    
    for (int i = 0; i < hitLog.Num(); i++)
    {
        if (hitLog[i].obj1 == 0 && hitLog[i].obj2 != SIDERAIL_HIT && !shotInfo.cueBallFirstOtherBallHit)
            shotInfo.cueBallFirstOtherBallHit = hitLog[i].obj2;
        if (shotInfo.cueBallFirstOtherBallHit && hitLog[i].obj2 == SIDERAIL_HIT)
            shotInfo.numRailTouchesAfterFirstHit++;
    }
    
	shotInfo.readyToFinalize = true;
}

void AMPRefereeBase::NextTeamShot(uint8 team)
{
	APool8BallGameMode* GM = Cast<APool8BallGameMode>(GetWorld()->GetAuthGameMode());
	if (team == 0)
	{
		int32 NewIdx = FirstTeamCurrentPlayerNum != 0 ? 0 : 2;
		FirstTeamCurrentPlayerNum = NewIdx;
		GM->NextShot(NewIdx);
		UE_LOG(LogTemp,Error,TEXT("New player idx: %i"),NewIdx)
	}
	else
	{
		int32 NewIdx = SecondTeamCurrentPlayerNum != 1 ? 1 : 3;
		SecondTeamCurrentPlayerNum = NewIdx;
		GM->NextShot(NewIdx);
		UE_LOG(LogTemp,Error,TEXT("New player idx: %i"),NewIdx)
	}
}

bool AMPRefereeBase::CanPlayerShoot(APoolPlayerControllerBase* player)
{
	if (shotInfo.currentPlayerNum >= 0 &&
        shotInfo.currentPlayerNum <= 1 &&
        playingPcs[shotInfo.currentPlayerNum] == player)
        	return true;
	else
		return false;
}

void AMPRefereeBase::NextShot(uint8 pc_num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
	if (gs)
	{
		if (firstShot)
			gs->SetRuleset(RULES_BEHIND_HEADSTRING);
		else
			gs->SetRuleset(RULES_ANYWHERE);
	}
}

void AMPRefereeBase::CheckRules()
{
	if (firstShot)
		CheckRules_FirstShot();
	else
		CheckRules_NotFirstShot();
}

void AMPRefereeBase::CheckRules_FirstShot()
{
	E_ShotResult& shotResult = shotInfo.shotResult;
	if (shotInfo.blackPocketed)
	{
		if (shotInfo.cueKnocked || shotInfo.cuePocketed)
			shotInfo.shotResult = E_ShotResult::LOSE;
		else
			shotInfo.shotResult = E_ShotResult::WIN;
	}
	else if (!shotInfo.blackPocketed)
	{
		if (shotInfo.cueBallFirstOtherBallHit == 0 ||
            shotInfo.numRailTouchesAfterFirstHit < 3)
            	shotInfo.shotResult = E_ShotResult::FOUL;
		else
			if (shotInfo.cuePocketed || shotInfo.cueKnocked)
				shotInfo.shotResult = E_ShotResult::FOUL;
			else
				if (shotInfo.lowPocketed || shotInfo.highPocketed)
					shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
				else
					shotInfo.shotResult = E_ShotResult::SHOT_LOST;
	}
}

void AMPRefereeBase::CheckRules_NotFirstShot()
{
	if (shotInfo.blackPocketed)
		CheckRules_NotFirstShot_BlackPocketed();
	else
		CheckRules_NotFirstShot_BlackNotPocketed();
}

void AMPRefereeBase::CheckRules_NotFirstShot_BlackPocketed()
{
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*>(pc->PlayerState);
    
	if (ps->nextTarget != E_Pool8BallNextTarget::BLACK_BALL ||
        shotInfo.cueKnocked ||
        shotInfo.cuePocketed)
        	shotInfo.shotResult = E_ShotResult::LOSE;
	else
		shotInfo.shotResult = E_ShotResult::WIN;
}

void AMPRefereeBase::CheckRules_NotFirstShot_BlackNotPocketed()
{
	if (shotInfo.cueBallFirstOtherBallHit == 0)
		shotInfo.shotResult = E_ShotResult::FOUL;
    
	else if (shotInfo.numRailTouchesAfterFirstHit == 0 &&
             !shotInfo.lowPocketed && !shotInfo.highPocketed)
             	shotInfo.shotResult = E_ShotResult::FOUL;
    
	else if (shotInfo.openTable)
		CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit();
    
	else if (!shotInfo.openTable)
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit();
}

void AMPRefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit()
{
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*>(pc->PlayerState);
	APool8BallPlayerController* other_pc = Cast<APool8BallPlayerController>(playingPcs[1 - shotInfo.currentPlayerNum]);
	APool8BallPlayerState* other_ps = dynamic_cast<APool8BallPlayerState*>(other_pc->PlayerState);
	if (shotInfo.cuePocketed || shotInfo.cueKnocked)
		shotInfo.shotResult = E_ShotResult::FOUL;
	else
		if (shotInfo.lowPocketed || shotInfo.highPocketed)
		{
			if (shotInfo.firstPocketedBall > 0 && shotInfo.firstPocketedBall < 8)
			{
				ps->ChangeNextTarget(E_Pool8BallNextTarget::LOW_BALLS);
				other_ps->ChangeNextTarget(E_Pool8BallNextTarget::HIGH_BALLS);
				shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
				shotInfo.openTable = false;
			}
			else
				if (shotInfo.firstPocketedBall > 8 && shotInfo.firstPocketedBall < 16)
				{
					ps->ChangeNextTarget(E_Pool8BallNextTarget::HIGH_BALLS);
					other_ps->ChangeNextTarget(E_Pool8BallNextTarget::LOW_BALLS);
					shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
					shotInfo.openTable = false;
				}
		}
		else
			if (!shotInfo.lowPocketed && !shotInfo.highPocketed)
				shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

void AMPRefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit()
{
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*>(pc->PlayerState);

	if (ps->nextTarget == E_Pool8BallNextTarget::LOW_BALLS)
	{
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted();
	}
	else if (ps->nextTarget == E_Pool8BallNextTarget::HIGH_BALLS)
	{
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted();
	}
	else if (ps->nextTarget == E_Pool8BallNextTarget::BLACK_BALL)
	{
		CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted();
	}
	else
	{
		ERROR_DEBUG_PRINT_FUNC_INFO("Invalid situation occurred")
	}
}

void AMPRefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted()
{
	if (!(shotInfo.cueBallFirstOtherBallHit > 0 || shotInfo.cueBallFirstOtherBallHit < 8))
		shotInfo.shotResult = E_ShotResult::FOUL;
	else if (shotInfo.lowPocketed)
		shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
	else
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

void AMPRefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted()
{
	if (!(shotInfo.cueBallFirstOtherBallHit > 8 || shotInfo.cueBallFirstOtherBallHit < 16))
		shotInfo.shotResult = E_ShotResult::FOUL;
	else if (shotInfo.highPocketed)
		shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
	else
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

void AMPRefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted()
{
	if (shotInfo.cueBallFirstOtherBallHit != 8)
		shotInfo.shotResult = E_ShotResult::FOUL;
	else
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
}

bool AMPRefereeBase::IsReadyToFinalize()
{
	return shotInfo.readyToFinalize;
}

void AMPRefereeBase::CheckRules_Finalize()
{
    //shotInfo.Clear(); //Needed to stop GM::Tick from calling Finalize
}

void AMPRefereeBase::RegisterMessage(E_MessageSeverity MessageSeverity, uint16 MessageID, bool ToCurrentPlayer, bool ShouldDeployMessage)
{
	TArray<APoolPlayerControllerBase*> Players;
	if (bIsTeamMode)
	{
		if (ToCurrentPlayer)
		{
			Players = shotInfo.currentPlayerNum == 0 ? FirstTeam : SecondTeam;
		}
		else
		{
			Players = shotInfo.currentPlayerNum == 0 ? SecondTeam : FirstTeam;
		}
	}
	else
	{
		Players.Add(ToCurrentPlayer ? playingPcs[shotInfo.currentPlayerNum] : playingPcs[1 - shotInfo.currentPlayerNum]);
	}
	for (int32 i =0; i < Players.Num();++i)
	{
		APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(Players[i]->PlayerState);
		if (PS)
		{
			PS->RegisterMessage(MessageSeverity,MessageID);
			UE_LOG(LogTemp,Error,TEXT("Deploy Message DeployMessages PS VALID"))
			if (ShouldDeployMessage) PS->Server_DeployMessages();
		} else UE_LOG(LogTemp,Error,TEXT("Deploy Message RegisterMessage PS NOT VALID"))
	}
	UE_LOG(LogTemp,Error,TEXT("Deploy Message RegisterMessage player controller count %i"),Players.Num())
}

void AMPRefereeBase::DeployMessages(bool ForBothPlayers)
{
	TArray<APoolPlayerControllerBase*> Players;
	if (bIsTeamMode)
	{
		if (ForBothPlayers )
		{
			Players.Append(FirstTeam); Players.Append(SecondTeam);
		}
		else
		{
			Players = shotInfo.currentPlayerNum == 0 ? FirstTeam : SecondTeam;
		}
	}
	else if (ForBothPlayers) Players = { playingPcs[0],playingPcs[1] };
	else Players.Add(playingPcs[shotInfo.currentPlayerNum]);
	for (int32 i = 0; i < Players.Num(); ++i)
	{
		APoolPlayerStateBase* CurrentPS = Cast<APoolPlayerStateBase>(Players[i]->PlayerState);
		if (CurrentPS)
		{
			CurrentPS->Server_DeployMessages();
			UE_LOG(LogTemp,Error,TEXT("Deploy Message DeployMessages PS VALID"))
		} else UE_LOG(LogTemp,Error,TEXT("Deploy Message DeployMessages PS NOT VALID"))
	}
	UE_LOG(LogTemp,Error,TEXT("Deploy Message DeployMessages player controller count %i"),Players.Num())
}

void AMPRefereeBase::TeamTravelToLoseScreen(uint8 TeamID)
{
	TArray<APoolPlayerControllerBase*> Players = TeamID == 0 ? FirstTeam : SecondTeam;
	for (int32 i = 0; i < Players.Num();++i)
	{
		Players[i]->ClientTravelToLevel("LoseScreen", "Authentication");
	}
}

void AMPRefereeBase::TeamTravelToWinScreen(uint8 TeamID)
{
	TArray<APoolPlayerControllerBase*> Players = TeamID == 0 ? FirstTeam : SecondTeam;
	for (int32 i =0; i < Players.Num();++i)
	{
		Players[i]->ClientTravelToLevel("WinScreen", "Authentication");
	}
}


void AMPRefereeBase::SetWinIdx(APoolPlayerControllerBase* pc_winner)
{
	win_pc_idx = pc_winner == playingPcs[1];
}

