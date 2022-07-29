// Fill out your copyright notice in the Description page of Project Settings.

#include "ShotInfo.h"
#include "ObjectGatherer.h"
#include "PoolGameModeBase.h"
#include "SefaPool/8Ball/Referee/MPRefereeBase.h"
#include "SefaPool/Shared/PoolGameStateBase.h"


AShotInfo::AShotInfo()
{
    PrimaryActorTick.bCanEverTick = true;
    shotEnded = false;
}

void AShotInfo::BeginPlay()
{
    Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects)
}

void AShotInfo::AddToHitLog(int obj1, int obj2, FVector pos, float time)
{
    if (!HasAuthority() || !inShot) return;
    if (obj1 > obj2)
    {
        int tmp = obj1;
        obj1 = obj2;
        obj2 = tmp;
    }
    
    FHitInfo info;
    info.obj1 = obj1;
    info.obj2 = obj2;
    info.time = time;
    info.pos = objects->physicsEngine->WorldToTableCoords(pos);
    hitLog.Add(info);
}

void AShotInfo::BallPocketed(int num)
{
    if (!HasAuthority() || !inShot || !ballsInGame[num] || newPocketedBalls[num] || newKnockedBalls[num]) return;
    if (num) firstPocketed = num;
    
    PRINTF((TEXT("ball pocketed")), Red)
    newPocketedBalls[num] = true;
    if (num)
        ballsInGame[num] = false;
    ballsActiveDuringShot[num] = false;
    bool allStopped = true;
    GAME_STATE_SYNC(gs, StorePocketedBall(num))
    for (int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
    {
        if (ballsInGame[i] && ballsActiveDuringShot[i])
        {
            allStopped = false;
        }
    }
    if (allStopped)
    {
        shotEnded = true;
    }
}

void AShotInfo::BallKnockedOff(int num)
{
    if (!HasAuthority() || !inShot || !ballsInGame[num] || newPocketedBalls[num] || newKnockedBalls[num]) return;
    //123 PRINTF((TEXT("Ball %d knocked"), num), Yellow)
    newKnockedBalls[num] = true;
    ballsActiveDuringShot[num] = false;
    bool allStopped = true;
    GAME_STATE_SYNC(gs, StorePocketedBall(num))
    for (int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
    {
        if (ballsInGame[i] && ballsActiveDuringShot[i])
        {
            allStopped = false;
        }
    }
    if (allStopped)
    {
        shotEnded = true;
    }
        
}
void AShotInfo::BallStopped(int num)
{
    if (!HasAuthority() || !inShot || !ballsInGame[num] || newPocketedBalls[num] || newKnockedBalls[num]) return;
    UE_LOG(LogTemp, Warning, TEXT("ball %d stopped"), num)
    ballsActiveDuringShot[num] = false;
    bool allStopped = true;
    int total_stopped = objects->physicsEngine->BallCountInGame;
    for (int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
    {
        if (ballsInGame[i] && ballsActiveDuringShot[i])
        {
            total_stopped --;
            allStopped = false;
        }
    }
    if (allStopped)
    {
        PRINTF((TEXT("Shot should end now")), White)
        shotEnded = true;
    }
}

void AShotInfo::Tick(float dt)
{
    if (!HasAuthority()) return;
    Super::Tick(dt);
    if (shotEnded)
    {
        PRINTF((TEXT("Shot Ended")), White)
        ShotEnded();
        shotEnded = false;
    }
}

void AShotInfo::BallMoved(int num)
{
    if (!HasAuthority() || !inShot || !ballsInGame[num] || newPocketedBalls[num] || newKnockedBalls[num]) return;
    ballsActiveDuringShot[num] = true;
}
void AShotInfo::ShotEnded()
{
    if (!HasAuthority()) return;
    
    inShot = false;
    
    APoolGameModeBase* gm = GetWorld()->GetAuthGameMode<APoolGameModeBase>();
    for (int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
    {
        if (newPocketedBalls[i])
            gm->Referee->ProcessBallPocketed(i, firstPocketed == i);
        if (newKnockedBalls[i])
            gm->Referee->ProcessBallKnockedOff(i);
    }
    if (networked)
        objects->ballinfo->StopClientUpload();
    else
    {
        if (objects && objects->hud && objects->hud->userBarWidget)
        {
            objects->hud->userBarWidget->BallsStopped();
        }
    }
    gm->Referee->ProcessHitLogAndFinalize(hitLog);
}
void AShotInfo::ResetForNextShot()
{
    if (!HasAuthority()) return;
    for (int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
    {
        ballsActiveDuringShot[i] = !i;
        newPocketedBalls[i] = false;
        newKnockedBalls[i] = false;
    }
    hitLog.Empty();
    inShot = true;
    firstPocketed = -1;
    shotStartTime = GetGameTimeSinceCreation();
    if (networked) objects->ballinfo->StartClientUpload();
    objects->balls[0]->Pocketed() = false;
    
}

float AShotInfo::GetTimeSinceShotBegan()
{
    return GetGameTimeSinceCreation() - shotStartTime;
}

void AShotInfo::SetNetworked(bool val)
{
    networked = val;
}


/*
void APool8BallPlayerController::InfoUpload_BallWasPocketed_Implementation(int num, bool first)
{
    APool8BallGameMode* gm = GetWorld()->GetAuthGameMode<APool8BallGameMode>();
    if (!gm) ERROR_DEBUG_PRINT_FUNC_INFO("Game mode not found");
    gm->ProcessBallPocketed(num, first);
}
void APool8BallPlayerController::InfoUpload_BallWasKnockedOff_Implementation(int num)
{
    APool8BallGameMode* gm = GetWorld()->GetAuthGameMode<APool8BallGameMode>();
    if (!gm) ERROR_DEBUG_PRINT_FUNC_INFO("Game mode not found");
    gm->ProcessBallKnockedOff(num);
    
}
void APool8BallPlayerController::InfoUpload_BallUpdate_Implementation(int num, FVector location, FRotator rotation)
{
    APool8BallGameMode* gm = GetWorld()->GetAuthGameMode<APool8BallGameMode>();
    if (!gm) ERROR_DEBUG_PRINT_FUNC_INFO("Game mode not found");
    //gm->ProcessBallUpdate(num, location, rotation);
}
void APool8BallPlayerController::InfoUpload_HitLog_Implementation(const TArray<int>& hitLog1, const TArray<int>& hitLog2)
{
    APool8BallGameMode* gm = GetWorld()->GetAuthGameMode<APool8BallGameMode>();
    if (!gm) ERROR_DEBUG_PRINT_FUNC_INFO("Game mode not found");
    gm->ProcessHitLogAndFinalize(hitLog1, hitLog2);
}*/


/*void APoolSPPlayerController::ServerPCReady_Implementation()
 {
     if (!this || !objects->table)
         ERROR_DEBUG_PRINT_FUNC
     GetWorld()->GetAuthGameMode<APoolSPGameMode>()->PCReady(this);
     //PRINTF((TEXT(__FUNCSIG__)), Yellow)
 }
 void APoolSPPlayerController::InfoUpload_HitLog_Implementation(const TArray<int>& hitLog1, const TArray<int>& hitLog2)
 {
     APoolSPGameMode* gm = GetWorld()->GetAuthGameMode<APoolSPGameMode>();
     if (!gm) ERROR_DEBUG_PRINT_FUNC_INFO("Game mode not found");
     gm->ProcessHitLogAndFinalize(this, hitLog1, hitLog2);
 }

 void APoolSPPlayerController::InfoUpload_BallWasPocketed_Implementation(int num, bool first)
 {
     APoolSPGameMode* gm = GetWorld()->GetAuthGameMode<APoolSPGameMode>();
     if (!gm) ERROR_DEBUG_PRINT_FUNC_INFO("Game mode not found");
     gm->ProcessBallPocketed(this, num, first);
 }
 void APoolSPPlayerController::InfoUpload_BallWasKnockedOff_Implementation(int num)
 {
     APoolSPGameMode* gm = GetWorld()->GetAuthGameMode<APoolSPGameMode>();
     if (!gm) ERROR_DEBUG_PRINT_FUNC_INFO("Game mode not found");
     gm->ProcessBallKnockedOff(this, num);
 }
*/
