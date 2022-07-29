// Fill out your copyright notice in the Description page of Project Settings.


#include "RefereeBase.h"

#include "SefaPool/8Ball/Pool8BallGameMode.h"
#include "SefaPool/8Ball/Pool8BallPlayerState.h"
#include "SefaPool/8Ball/BallsGatherer/BallManagerBase.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"

ARefereeBase::ARefereeBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	FirstBallIndex = 0;
	BallCountInGame = 16;
}

// Called when the game starts or when spawned
void ARefereeBase::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(objects)
    
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        backend_data = Cast<UBaseBackendInfo>(gi->backend_data);
    }
    
	PlayingPc = Cast<APoolPlayerControllerBase>(GetWorld()->GetFirstPlayerController());
    CurrentIdleTime = GetGameTimeSinceCreation();
    firstShot = true;
    CountOfPocketedBalls = 0;
    TotalTimeInGame = 0.0f;
    ShotBeginTime = ShotEndTime = AimingStartTime = -1.0f;
	objects->BallManager->FirstBallIndex = FirstBallIndex;
	objects->BallManager->BallsCountInGame = BallCountInGame;
	objects->BallManager->SetBallsForGM();
	// BallManager = GetWorld()->SpawnActor<ABallManagerBase>(ABallManagerBase::StaticClass());
}

float ARefereeBase::GetCurrentTotalInGameTime()
{
    
    if (AimingStartTime < 0.0f && ShotBeginTime < 0.0f && ShotEndTime < 0.0f)
    {
        return TotalTimeInGame;
    }
    if (AimingStartTime > 0.0f && ShotBeginTime < 0.0f && ShotEndTime < 0.0f)
    {
        return TotalTimeInGame + GetGameTimeSinceCreation() - AimingStartTime;
    }
    if (AimingStartTime > 0.0f && ShotBeginTime > 0.0f && ShotEndTime < 0.0f)
    {
        return TotalTimeInGame + GetGameTimeSinceCreation() - (firstShot ? AimingStartTime : ShotBeginTime);
    }
    if (AimingStartTime > 0.0f && ShotBeginTime > 0.0f && ShotEndTime < 0.0f)
    {
        return TotalTimeInGame + ShotEndTime - (firstShot ? AimingStartTime : ShotBeginTime);
    }
    
    
    return CurrentIdleTime - TotalIdleTime;
}



void ARefereeBase::OnPlayerStageStart()
{
	APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(GetWorld()->GetFirstPlayerController()->PlayerState);
	//GetWorld()->GetTimerManager().SetTimer(FirstPlayerPlayDelay, this, &ARefereeBase::OnPlayTimeEnd,PS->cueInfo.GetTime() + 3.0f);
    AimingStartTime = GetGameTimeSinceCreation();
    if (firstShot) { return;}
    if (GetLocalRole() < ROLE_Authority) { return; }
    TotalIdleTime += ( GetGameTimeSinceCreation() - CurrentIdleTime);
}

void ARefereeBase::OnPlayerStageEnd()
{
    if (GetLocalRole() < ROLE_Authority) { return;}
	GetWorld()->GetTimerManager().ClearTimer(FirstPlayerPlayDelay);
    ShotBeginTime = GetGameTimeSinceCreation();
    OnReactionTimeUpdate(ShotBeginTime - AimingStartTime);
    if (firstShot) {
        TotalIdleTime += ( GetGameTimeSinceCreation() - CurrentIdleTime);
        return;
    }
}

void ARefereeBase::OnReactionTimeUpdate(float newRtime)
{
}

void ARefereeBase::OnBallsStopped()
{
    if (GetLocalRole() < ROLE_Authority) { return;}
    ShotEndTime = GetGameTimeSinceCreation();
    CurrentIdleTime = GetGameTimeSinceCreation();
    float TotalOld = TotalTimeInGame;
    TotalTimeInGame += ShotEndTime - (firstShot ? ShotBeginTime : AimingStartTime);
    
    firstShot = false;
    ShotBeginTime = ShotEndTime = AimingStartTime = -1.0f;
}

void ARefereeBase::OnPlayTimeEnd()
{
	PlayingPc->ShotTimeHasEnded();
    TArray<FHitInfo> emptyHitLog;
    if (GetLocalRole() < ROLE_Authority) { return;}
    TotalTimeInGame += GetGameTimeSinceCreation() - AimingStartTime;
    ShotBeginTime = ShotEndTime = AimingStartTime = -1.0f;
    ProcessHitLogAndFinalize(emptyHitLog);
    if (firstShot) {  firstShot = false; return;}
}

void ARefereeBase::ProcessBallPocketed(int num, bool first)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
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
	UE_LOG(LogTemp, Error, TEXT("ProcessBallPocketed"))
}

void ARefereeBase::ProcessBallKnockedOff(int num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
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

void ARefereeBase::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
    OnBallsStopped();
}

bool ARefereeBase::CanPlayerShoot(APoolPlayerControllerBase* player)
{
	UE_LOG(LogTemp,Error,TEXT("CanPlayerShoot"))
	return true;
}

void ARefereeBase::NextShot(uint8 pc_num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
	if (gs)
	{
		if (firstShot)
			gs->SetRuleset(RULES_BEHIND_HEADSTRING);
		else
			gs->SetRuleset(RULES_ANYWHERE);
	}
	UE_LOG(LogTemp,Error,TEXT("NextShot"))
}

uint8 ARefereeBase::GetFirstPCPlayedNum()
{
	return FMath::RandRange(0,1);
}

void ARefereeBase::ShuffleAndArrangeBalls()
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
	// triangular numbers maffs starts here
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
		//balls[arrangeableBalls[i]]->mesh->SetWorldLocation(loc, false, nullptr, ETeleportType::TeleportPhysics);
		//balls[arrangeableBalls[i]]->mesh->SetWorldRotation(rot, false, nullptr, ETeleportType::TeleportPhysics);
		//InfoUpload_LoadBallTransform(arrangeableBalls[i], loc, rot, false);
	}
    GS->ReturnCueBall();
	/*InfoUpload_LoadBallTransform(0,
		GetCueBallDefaultLocationOnTable() + FVector(0.0f, 0.0f, 50.0f * objects->balls[0]->scale),
		FRotator(0.0f), 
		false, 
		true, 
		true,
		false);*/
}

void ARefereeBase::OnGameStart()
{
    APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(GetWorld()->GetFirstPlayerController()->PlayerState);
    if (PS)
    {
        PS->ballInHand = true;
        PS->UpdateClient();
    }
}



void ARefereeBase::CheckRules()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules"))
}

void ARefereeBase::CheckRules_FirstShot()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_FirstShot"))
}

void ARefereeBase::CheckRules_NotFirstShot()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot"))
}

void ARefereeBase::CheckRules_NotFirstShot_BlackPocketed()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot_BlackPocketed"))
}

void ARefereeBase::CheckRules_NotFirstShot_BlackNotPocketed()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot_BlackNotPocketed"))
}

void ARefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit"))
}

void ARefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit"))
}

void ARefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted"))
}

void ARefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted"))
}

void ARefereeBase::CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted"))
}

void ARefereeBase::CheckRules_Finalize()
{
	UE_LOG(LogTemp,Error,TEXT("CheckRules_Finalize"))
}

bool ARefereeBase::IsReadyToFinalize()
{
	return true;
}


void ARefereeBase::GameResults()
{
    TArray<uint8> replayData;
    objects->ReplayManager->OnGameEnd(replayData);
    if (replayData.Num() > 0)
    {
        FString encodedReplay = FBase64::Encode(replayData.GetData(),
                                                replayData.Num());
        ProcessBase64Replay(encodedReplay);
    }
	APool8BallGameMode* GM = Cast<APool8BallGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->ReturnSpectatorToMenu();
	}
}


void ARefereeBase::ProcessBase64Replay(const FString& replay)
{
    
}

void ARefereeBase::PlayerExited(FString exiting_player_id)
{

}

void ARefereeBase::UpdateTargetableBalls(APoolPlayerStateBase* PlayerState)
{
}

cool_json_t ARefereeBase::CreateConsolidationJsonForPlayer(FString user_id)
{
    
    if (!backend_data) return cool_json_t();
    
    FBackendPlayerInfo* player_info = backend_data->GetPlayerInfo(user_id);
    if (!player_info)
        return cool_json_t();
    
    cool_json_t json;
    json["user_id"] = player_info->user_id;
    UE_LOG(LogTemp, Warning, TEXT("Create subjson for cues"));
    json["reaction_time"] = player_info->totalReactionTime;
    json["shots_played"] = player_info->shotsPlayed;
    json["cues"] = cool_json_t::array();
    for (int i = 1; i < PREDEFINED_CUES_N + 1; ++i)
    {
        if (player_info->durability.cue_durabilities[i] != -1)
        {
            cool_json_t cue_json;
            cue_json["cue_id"] = FString::FromInt(i);
            cue_json["recharged"] = FString::FromInt(player_info->durability.cue_recharges[i]);
            cue_json["used"] = FString::FromInt(player_info->durability.cue_usages[i]);
            json["cues"].push_back(cue_json);
        }
    }
    json["chalks"] = cool_json_t::array();
    UE_LOG(LogTemp, Warning, TEXT("Create subjson for chalk"));
    for (int i = 1; i < PREDEFINED_CHALKS_N + 1; ++i)
    {
        if (player_info->durability.chalk_durabilities[i] != -1)
        {
            cool_json_t chalk_json;
            chalk_json["chalk_id"] = FString::FromInt(i);
            chalk_json["recharged"] = FString::FromInt(player_info->durability.chalk_recharges[i]);
            chalk_json["used"] = FString::FromInt(player_info->durability.chalk_usages[i]);
            json["chalks"].push_back(chalk_json);
        }
    }
    return json;
}
