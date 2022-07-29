// Fill out your copyright notice in the Description page of Project Settings.


#include "PocketAsManyUCanReferee.h"


#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"
#include "SefaPool/SinglePlayer/PoolSPGameMode.h"
#include "SefaPool/SinglePlayer/PoolSPPlayerController.h"

class APoolSPGameMode;

APocketAsManyUCanReferee::APocketAsManyUCanReferee()
{
	CountOfPocketedBalls = 0;
	BallsPocketedOnPreviousStage = 0;
}


void APocketAsManyUCanReferee::ShuffleAndArrangeBalls()
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

void APocketAsManyUCanReferee::BeginPlay()
{
	Super::BeginPlay();
	objects->ReplayManager->InitializeGameMode(ERecordedGameMode::PocketAsManyUCan);
}

void APocketAsManyUCanReferee::OnPlayerStageStart()
{
    Super::OnPlayerStageStart();
    if (firstShot) {  firstShot = false; return;}
	/*if (!BallsPocketedOnPreviousStage)
	{
		APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
		SPplayingPc->ClientTravelToLoseScreen();
		UE_LOG(LogTemp, Error, TEXT("BallsPocketedOnPreviousStage is 0, score: %i "), CountOfPocketedBalls)
		return;
	}*/
	BallsPocketedOnPreviousStage = 0;
	APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(PlayingPc->PlayerState);
    newPocketedBalls.Empty();
}

void APocketAsManyUCanReferee::ProcessBallPocketed(int num, bool first)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
	if (num)
	{
		CountOfPocketedBalls++;
		BallsPocketedOnPreviousStage++;
        newPocketedBalls.Add(num);
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

void APocketAsManyUCanReferee::ProcessBallKnockedOff(int num)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        uint16 message = MESSAGE_KNOCKED_OFF_CUE_BALL + num;
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, message);
    }
    foul = true;
}


void APocketAsManyUCanReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
    Super::ProcessHitLogAndFinalize(hitLog);
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->Server_DeployMessages();
        ps->ballInHand = false;
        ps->UpdateClient();
    }
    APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
    
    if (!foul && !BallsPocketedOnPreviousStage)
    {
        ps->SendImmediateMessage(E_MessageSeverity::PRIMARY, MESSAGE_CONTEST_NO_POCKETED);
    }
    if (foul || !BallsPocketedOnPreviousStage)
    {
        SPplayingPc->ClientTravelToLevel("Authentication", "LoseScreen");
        GameResults();
        return;
    }
    
    bool legalShot = firstShot;
    
    for (auto ballNum : newPocketedBalls)
    {
        int firstTouchIdx = -1;
        for (int i = 0; i < hitLog.Num(); i++)
            if (hitLog[i].obj1 == ballNum || hitLog[i].obj2 == ballNum)
                firstTouchIdx = i;
        if (firstTouchIdx == -1)
            continue;
        
        bool touchedRailBeforePocketed = false;
        for (int i = firstTouchIdx; i < hitLog.Num(); i++)
            if (hitLog[i].obj1 == ballNum && hitLog[i].obj2 == SIDERAIL_HIT)
                touchedRailBeforePocketed = true;
        if (touchedRailBeforePocketed)
        {
            legalShot = true;
            PRINTF((TEXT("Legal shot because touched rail")), Green)
            break;
        }
        bool hasCueBallTouchedCushionBeforeTargetBallHasBeenPocketed = false;
        for (int i = 0; i < firstTouchIdx; i++)
            if (hitLog[i].obj1 == 0 && hitLog[i].obj2 == SIDERAIL_HIT)
                hasCueBallTouchedCushionBeforeTargetBallHasBeenPocketed = true;
        if (hasCueBallTouchedCushionBeforeTargetBallHasBeenPocketed)
        {
            legalShot = true;
            PRINTF((TEXT("Legal shot because cue ball touched rail")), Green)
            break;
        }
    }
    if (!legalShot)
    {
        ps->SendImmediateMessage(E_MessageSeverity::PRIMARY, MESSAGE_CONTEST_ILLEGAL_SHOT);
        SPplayingPc->ClientTravelToLevel("Authentication", "LoseScreen");
        GameResults();
        return;
    }
    
    
    
	bool inGame = false;
	for (int i = 1; i < 16; i++)
	{
		if (dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState())->ballsInGame[i])
		{
			inGame = true;
			break;
		}
	}
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

int APocketAsManyUCanReferee::ContestId()
{
    return static_cast<int> (EContestType::CONTEST_CUSHION);
}
