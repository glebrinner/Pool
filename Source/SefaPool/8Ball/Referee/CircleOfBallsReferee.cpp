// Fill out your copyright notice in the Description page of Project Settings.


#include "CircleOfBallsReferee.h"

#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/SinglePlayer/PoolSPGameMode.h"
#include "SefaPool/SinglePlayer/PoolSPPlayerController.h"

ACircleOfBallsReferee::ACircleOfBallsReferee()
{
    PrimaryActorTick.bCanEverTick = false;
    bMakeRandomCircleAngle = true;
    CircleRadiusInTableCoords = CIRCLE_OF_BALLS_RADIUS;
    CircleCenter = FVector2D(0.5f,1.f);
}

void ACircleOfBallsReferee::BeginPlay()
{
    Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects);
    CircleRadiusInUnrealUnits = (objects->physicsEngine->TableCoordsToWorld(FVector2D(0.f,0.f)) - objects->physicsEngine->TableCoordsToWorld(FVector2D(CircleRadiusInTableCoords,0.f))).Size();
    CircleCenterIn3D = objects->physicsEngine->TableCoordsToWorld(CircleCenter);
    CircleCenterIn3D.Z = 79.f;
    foul = false;
    BallsPocketedOnPreviousStage = 0;
	objects->ReplayManager->InitializeGameMode(ERecordedGameMode::CircleOfBalls);
    UE_LOG(LogTemp, Error, TEXT("CircleRadiusInUnrealUnits %d"), CircleRadiusInTableCoords)
}

void ACircleOfBallsReferee::OnPlayTimeEnd()
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_TIME_ENDED);
    }
    foul = true;
    
    Super::OnPlayTimeEnd();
}

void ACircleOfBallsReferee::NextShot(uint8 pc_num)
{
    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
    if (gs)
    {
        gs->SetRuleset(RULES_CIRCLE_OF_BALLS);
    }
}

bool ACircleOfBallsReferee::IsPocketBallInCircle()
{
    ABall* PocketBallInstance = objects->balls[0];
    TArray<FHitResult> HitResults;
    FCollisionQueryParams QueryParams;
    for (uint8 i =1;i<16;++i)
    {
        QueryParams.AddIgnoredActor(objects->balls[i]);
    }
    if (GetWorld()->SweepMultiByChannel(HitResults,CircleCenterIn3D,CircleCenterIn3D+0.0001f,FQuat::Identity,
        ECollisionChannel::ECC_Visibility,FCollisionShape::MakeSphere(CircleRadiusInUnrealUnits),QueryParams))
    {
        UE_LOG(LogTemp,Error,TEXT("Sweep hit %i Objects"),HitResults.Num())
        for (uint8 i =0;i< HitResults.Num();++i)
        {
            if (HitResults[i].Actor.Get())
            {
                if (PocketBallInstance == HitResults[i].Actor.Get())
                {
                    return true;
                }
            }
        }
    }
    //DrawDebugSphere(GetWorld(),CircleCenterIn3D,CircleRadiusInUnrealUnits,12,FColor::Red,false,125.f);
    return false;
}
/*
void ACircleOfBallsReferee::OnPlayerStageStart()
{
	Super::OnPlayerStageStart();
	APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(PlayingPc->PlayerState);
	UpdateTargetableBalls(PS);
}
*/


void ACircleOfBallsReferee::ProcessBallPocketed(int num, bool first)
{
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    uint16 message = MESSAGE_POCKETED_CUE_BALL + num;
    if (ps && !num)
        ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
    if (num == 0)
    {
        foul = true;
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_POCKETED_CUE_BALL);
        return;
    }
    BallsPocketedOnPreviousStage++;
    CountOfPocketedBalls++;
    UE_LOG(LogTemp,Error,TEXT("Pocketed balls count: %i"),CountOfPocketedBalls)
}

void ACircleOfBallsReferee::ProcessBallKnockedOff(int num)
{
    // cue => issue foul/loss
    // other => return to point
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    uint16 message = MESSAGE_KNOCKED_OFF_CUE_BALL + num;
    ps->RegisterMessage(E_MessageSeverity::SECONDARY, message);
    
    APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
    
    APoolSPPlayerController* SPplayingPc = Cast<APoolSPPlayerController>(PlayingPc);
    if (SPplayingPc)
    {
        foul = true;
        ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_KNOCKED_OFF_CUE_BALL);
        return;
    }
}

void ACircleOfBallsReferee::ShuffleAndArrangeBalls()
{
    FVector CircleShuffleCenter = objects->physicsEngine->TableCoordsToWorld(CircleCenter);
    CircleShuffleCenter.Z = objects->physicsEngine->ballRestingOnTableZ;
    SetActorLocation(CircleShuffleCenter);
    const float AngleDelta = 360.f / 15.f;
    FRotator BallSpawnDirection = FVector::ForwardVector.Rotation();
    FRotator BallSpawnDirectionBak = BallSpawnDirection;
    
    if (bMakeRandomCircleAngle) BallSpawnDirection.Yaw = FMath::RandRange(0.f,360.f);
    if (firstShot)
    {
        PlaceBallOnTablePosition(CircleCenter);
        PRINTF((TEXT("placing cue ball")), Green)
    }
    for (uint8 i=1; i < 16 && !firstShot; i++)
    {
        FVector BallSpawnLocation = CircleShuffleCenter + BallSpawnDirection.Vector()*CircleRadiusInUnrealUnits;
        BallSpawnLocation.Z = objects->physicsEngine->ballRestingOnTableZ;
        if (!objects->physicsEngine->IsPosFreeFromCueBall(objects->physicsEngine->WorldToTableCoords(BallSpawnLocation)))
        {
            APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
            if (ps)
            {
                ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_CONTEST_CUE_BALL_INTERFERES);
            }
            foul = true;
            return;
        }
        BallSpawnDirection.Yaw+=AngleDelta;
    }
    
    BallSpawnDirection = BallSpawnDirectionBak;
    for (uint8 i=1; i <16;i++)
    {
        FVector BallSpawnLocation = CircleShuffleCenter + BallSpawnDirection.Vector()*CircleRadiusInUnrealUnits;
        BallSpawnLocation.Z = objects->physicsEngine->ballRestingOnTableZ;
        objects->physicsEngine->MoveBallAndStop(i,BallSpawnLocation,FRotator::ZeroRotator);
        // DrawDebugSphere(GetWorld(),BallSpawnLocation,15.f,12,FColor::Red,false,5.f);
        BallSpawnDirection.Yaw+=AngleDelta;
    }
}

void ACircleOfBallsReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
    Super::ProcessHitLogAndFinalize(hitLog);
    
    if (GetLocalRole() < ROLE_Authority) return;
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*> (PlayingPc->PlayerState);
    if (ps)
    {
        PRINTF((TEXT("Sending messages to client")), Red)
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
    BallsPocketedOnPreviousStage = 0;
    
    if (!IsPocketBallInCircle())
    {
        if (ps)
        {
            ps->SendImmediateMessage(E_MessageSeverity::PRIMARY, MESSAGE_CIRCLE_OF_BALLS_EXITED_CIRCLE);
        }
            
        if (SPplayingPc)
        {
            SPplayingPc->ClientTravelToLevel("Authentication", "LoseScreen");
            GameResults();
        }
        return;
    }
    int newBallsHit = 0;
    int previousBall = 0;
    for (uint8 i = 0; i < hitLog.Num(); ++i)
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
        
        if (SPplayingPc)
            SPplayingPc->ClientTravelToLevel("Authentication", "LoseScreen");
        GameResults();
        return;
    }
    
    // if (GetWorld()->SweepMultiByChannel(GetWorld(),))
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


void ACircleOfBallsReferee::PlaceBallOnTablePosition(FVector2D TablePosition)
{
    FVector TableCoordinate = objects->physicsEngine->TableCoordsToWorld(TablePosition);
    objects->physicsEngine->MoveBallAndStop(0,TableCoordinate,FRotator::ZeroRotator);
}


int ACircleOfBallsReferee::ContestId()
{
    return static_cast<int> (EContestType::CONTEST_CIRCLE_OF_BALLS);
}
