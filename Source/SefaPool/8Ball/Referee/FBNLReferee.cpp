// Fill out your copyright notice in the Description page of Project Settings.


#include "FBNLReferee.h"



#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/SinglePlayer/PoolSPGameMode.h"
#include "SefaPool/SinglePlayer/PoolSPPlayerController.h"



AFBNLReferee::AFBNLReferee()
{
    CountOfPocketedBalls = 0;
	BallsPocketedOnPreviousStage = 0;
    firstShot_nofoul = true;
}


void AFBNLReferee::BeginPlay()
{
	Super::BeginPlay();
    foul = false;
    objects->ReplayManager->InitializeGameMode(ERecordedGameMode::FBNL);
}

void AFBNLReferee::OnPlayerStageStart()
{
	Super::OnPlayerStageStart();
	BallsPocketedOnPreviousStage = 0;
    APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(PlayingPc->PlayerState);
    UpdateTargetableBalls(PS);
}

void AFBNLReferee::UpdateTargetableBalls(APoolPlayerStateBase* PlayerState)
{
    TArray<uint8> TargetableBalls;
    for (uint8 i = 0; i < BallCountInGame; ++i)
    {
        TargetableBalls.Add(i);
    }
    if (PlayerState)
    {
        PlayerState->ClientSetTargetableBalls(TargetableBalls);
    }else UE_LOG(LogTemp,Error,TEXT("Player state not valid"))
    //return TargetableBalls;
}

void AFBNLReferee::OnPlayTimeEnd()
{
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_TIME_ENDED);
    }
    foul = true;
    Super::OnPlayTimeEnd();
}

void AFBNLReferee::ProcessBallPocketed(int num, bool first)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    
	if (num)
	{
		CountOfPocketedBalls++;
		BallsPocketedOnPreviousStage++;
        if (ps)
        {
            uint16 message = MESSAGE_POCKETED_CUE_BALL + num;
            ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
        }
	}
	else
	{
        foul = true;
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_POCKETED_CUE_BALL);
	}
}

void AFBNLReferee::ProcessBallKnockedOff(int num)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        uint16 message = MESSAGE_KNOCKED_OFF_CUE_BALL + num;
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, message);
    }
    foul = true;
    
}

void AFBNLReferee::ShuffleAndArrangeBalls()
{
    if (!HasAuthority()) return;
    APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
    int leftCornerBallNum = FMath::RandRange(1, 7);
    int rightCornerBallNum = FMath::RandRange(9, 15);
    if (FMath::RandRange(0, 1))
    {
        int tmp = leftCornerBallNum;
        leftCornerBallNum = rightCornerBallNum;
        rightCornerBallNum = tmp;
    }
    TArray<int> arrangeableBalls;
    for (int i = 1; i < 16; i++)
    {
        if (i != 8 && i != leftCornerBallNum && i != rightCornerBallNum)
            arrangeableBalls.Add(i);
    }
    for (int i = arrangeableBalls.Num() - 1; i > 1; i--)
    {
        int randIdx = FMath::RandRange(0, i);
        int tmp = arrangeableBalls[i];
        arrangeableBalls[i] = arrangeableBalls[randIdx];
        arrangeableBalls[randIdx] = tmp;
    }
    arrangeableBalls.Insert(8, 4);
    arrangeableBalls.Insert(leftCornerBallNum, 10);
    arrangeableBalls.Insert(rightCornerBallNum, 14);
    FVector v1 = GS->GetFoulBallDefaultReturnLocationOnTable() - GS->GetCueBallDefaultLocationOnTable();
    v1.Normalize();
    v1 *= 100.0f * objects->balls[0]->scale;
    FVector v2 = v1;
    v1 = v1.RotateAngleAxis(-30.0f, FVector(0.0f, 0.0f, 1.0f));
    v2 = v2.RotateAngleAxis(30.0f, FVector(0.0f, 0.0f, 1.0f));
    
    if (firstShot)
        GS->ReturnCueBall();
    
    for (uint8 i = 1; i < 15 && !firstShot; i++)
    {
        int row = int((sqrt(1.0f + 8.0f * i) - 1.0f) / 2.0f);
        int v1Steps = (row * (row + 3)) / 2 - i;
        int v2Steps = i - (row * (row + 1)) / 2;
        FVector loc = GS->GetFoulBallDefaultReturnLocationOnTable() + FVector(0.0f, 0.0f, 50.0f * objects->balls[0]->scale);
        loc += v1 * v1Steps + v2 * v2Steps;
        
        if (!objects->physicsEngine->IsPosFreeFromCueBall(objects->physicsEngine->WorldToTableCoords(loc)))
        {
            APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
            if (ps)
            {
                ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_CONTEST_CUE_BALL_INTERFERES);
            }
            foul = true;
            return;
        }
    }
    
    for (int i = 0; i < 15; i++)
    {
        int row = int((sqrt(1.0f + 8.0f * i) - 1.0f) / 2.0f);
        int v1Steps = (row * (row + 3)) / 2 - i;
        int v2Steps = i - (row * (row + 1)) / 2;
        FVector loc = GS->GetFoulBallDefaultReturnLocationOnTable() + FVector(0.0f, 0.0f, 50.0f * objects->balls[0]->scale);
        loc += v1 * v1Steps + v2 * v2Steps;
        FRotator rot = FRotator(FMath::RandRange(0.0f, 360.0f),
            FMath::RandRange(0.0f, 360.0f),
            FMath::RandRange(0.0f, 360.0f));
        objects->physicsEngine->MoveBallAndStop(arrangeableBalls[i], loc, rot);
    }

}

void AFBNLReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
    Super::ProcessHitLogAndFinalize(hitLog);
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->Server_DeployMessages();
        ps->ballInHand = false;
        ps->UpdateClient();
    }
    
    if (!foul && !BallsPocketedOnPreviousStage && !firstShot_nofoul)
    {
        ps->SendImmediateMessage(E_MessageSeverity::PRIMARY, MESSAGE_CONTEST_NO_POCKETED);
    }
    if (foul || (!BallsPocketedOnPreviousStage && !firstShot_nofoul))
    {
        APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
        SPplayingPc->ClientTravelToLevel("Authentication", "LoseScreen");
        GameResults();
        return;
    }
    
    firstShot_nofoul = false;
    
    
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
        ShuffleAndArrangeBalls();
        if (foul)
        {
            if (ps) ps->Server_DeployMessages();
            SPplayingPc->ClientTravelToLevel("Authentication", "LoseScreen");
            GameResults();
            return;
        }
    }
	
	
    APoolSPGameMode* GM = Cast<APoolSPGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
        GM->NextShot(SPplayingPc);
}


bool AFBNLReferee::IsCueBallInTriangle()
{
	return false;
}
int AFBNLReferee::ContestId()
{
    return static_cast<int> (EContestType::CONTEST_FBNL);
}
