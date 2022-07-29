// Fill out your copyright notice in the Description page of Project Settings.


#include "LineUpReferee.h"

#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/SinglePlayer/PoolSPGameMode.h"
#include "SefaPool/SinglePlayer/PoolSPPlayerController.h"

ALineUpReferee::ALineUpReferee()
{
	CueBallPosition = FVector2D(0.25f,0.25f);
	LineUpStartPosition = FVector2D(0.5f,0.0f);
    LineUpEndPosition = FVector2D(0.5f, 2.0f);
	DistanceBetweenBallsInTableCoords = 0.15f;
	BallsPocketedOnPreviousStage = 0;
	firstShot = true;
    CountOfPocketedBalls = 0;
	FirstBallIndex = 0;
}

void ALineUpReferee::NextShot(uint8 pc_num)
{
    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
    if (gs)
    {
        gs->SetRuleset(RULES_ANYWHERE);
    }
}



void ALineUpReferee::BeginPlay()
{
	Super::BeginPlay();
    DistanceBeetweenBallsInUnrealUnits = (objects->physicsEngine->TableCoordsToWorld(LineUpStartPosition) - objects->physicsEngine->TableCoordsToWorld(LineUpEndPosition)).Size() / 14.0f;
    foul = false;
	objects->ReplayManager->InitializeGameMode(ERecordedGameMode::LineUp);
	UE_LOG(LogTemp,Error,TEXT("Distance Between Balls in UE units %f") ,DistanceBeetweenBallsInUnrealUnits)
}

void ALineUpReferee::ShuffleAndArrangeBalls()
{
	APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
        for (uint8 i = 0; i < BallCountInGame; i++)
		{
			objects->balls[i]->pocketed = false;
			//GS->StorePocketedBall(Iterator);
			GS->ballsInGame[i] = true;
		}
	}
	
	FVector LineUpStartPoint = objects->physicsEngine->TableCoordsToWorld(LineUpStartPosition);
	LineUpStartPoint.Z = objects->physicsEngine->ballRestingOnTableZ;
    
    float BallRadius = objects->physicsEngine->ballR;
    
    float ballRinTableCoords = objects->physicsEngine->WorldToTableCoords(objects->physicsEngine->TableCoordsToWorld(FVector2D(0.0f, 0.0f)) +
                                                                          FVector(BallRadius, 0.0f, 0.0f)).Size();
    
    
	SetActorLocation(LineUpStartPoint);
	float CurrentDistance = 0;
	FVector BallSpawnDirection = -FVector::ForwardVector;
    
    if (firstShot)
        PlaceBallOnTablePosition(CueBallPosition);
    
    FVector2D StartPosition2D = LineUpStartPosition + (LineUpEndPosition - LineUpStartPosition).GetSafeNormal() * ballRinTableCoords;
    FVector2D EndPosition2D = LineUpEndPosition - (LineUpEndPosition - LineUpStartPosition).GetSafeNormal() * ballRinTableCoords;
    FVector StartPosition3D = objects->physicsEngine->TableCoordsToWorld(StartPosition2D);
    FVector EndPosition3D = objects->physicsEngine->TableCoordsToWorld(EndPosition2D);
    
    StartPosition3D.Z = EndPosition3D.Z = objects->physicsEngine->ballRestingOnTableZ;
    
    
    for (uint8 i = 1; i < BallCountInGame && !firstShot; i++)
    {
        if (!objects->physicsEngine->IsPosFreeFromCueBall(StartPosition2D + (i - 1) * (EndPosition2D - StartPosition2D) / 14.0f))
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
    
	for (uint8 i=1; i <BallCountInGame;i++)
	{
        FVector BallSpawnLocation = StartPosition3D + (i - 1) * (EndPosition3D - StartPosition3D) / 14.0f;
		objects->physicsEngine->MoveBallAndStop(i,BallSpawnLocation,FRotator::ZeroRotator);
		// DrawDebugSphere(GetWorld(),BallSpawnLocation,15.f,12,FColor::Red,false,15.f,0,2.f);
		CurrentDistance += DistanceBeetweenBallsInUnrealUnits;
	}
}

void ALineUpReferee::PlaceBallOnTablePosition(FVector2D TablePosition)
{
	FVector TableCoordinate = objects->physicsEngine->TableCoordsToWorld(TablePosition);
	objects->physicsEngine->MoveBallAndStop(0,TableCoordinate,FRotator::ZeroRotator);
}
/*
void ALineUpReferee::GameResults()
{
	Super::GameResults();
	// objects->ReplayManager->LoadMatchReplay();
}*/


void ALineUpReferee::OnPlayerStageStart()
{
    Super::OnPlayerStageStart();
	if (firstShot) {return;}
	BallsPocketedOnPreviousStage = 0;
	APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(PlayingPc->PlayerState);
}

void ALineUpReferee::OnPlayTimeEnd()
{
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_TIME_ENDED);
    }
    foul = true;
    Super::OnPlayTimeEnd();
}


void ALineUpReferee::ProcessBallPocketed(int num, bool first)
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
        if (ps)
        {
            ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_POCKETED_CUE_BALL);
        }
        foul = true;
		
	}
}


void ALineUpReferee::ProcessBallKnockedOff(int num)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        uint16 message = MESSAGE_KNOCKED_OFF_CUE_BALL + num;
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, message);
    }
    foul = true;
}

void ALineUpReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
    Super::ProcessHitLogAndFinalize(hitLog);
    if (GetLocalRole() < ROLE_Authority) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
    if (ps)
    {
        ps->Server_DeployMessages();
        ps->ballInHand = false;
        ps->UpdateClient();
    }
    
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
    int newBallsHit = 0;
    int previousBall = 0;
    for (uint8 i = 0; i < hitLog.Num();++i)
    {
        if (hitLog[i].obj1 == 0 && hitLog[i].obj2 && hitLog[i].obj2 < 16 && hitLog[i].obj2 != previousBall)
        {
            newBallsHit++;
            previousBall = hitLog[i].obj2;
        }
    }
    
    if (newBallsHit > 1)
    {
        if (ps)
        {
            ps->SendImmediateMessage(E_MessageSeverity::PRIMARY, MESSAGE_CONTEST_MORE_THAN_ONE_BALL);
        }
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


int ALineUpReferee::ContestId()
{
    return static_cast<int> (EContestType::CONTEST_LINE_UP);
}
