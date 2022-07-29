// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SefaPool/8Ball/Pool8BallGameMode.h"
#include "SefaPool/Shared/ShotInfo.h"
#include "GameFramework/Actor.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "RefereeBase.generated.h"

UCLASS()
class SEFAPOOL_API ARefereeBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ARefereeBase();

protected:
	virtual void BeginPlay() override;
	AObjectGatherer* objects;

    float TotalTimeInGame;

    float TotalIdleTime;

    float CurrentIdleTime;
    
    float AimingStartTime;
    float ShotBeginTime;
    float ShotEndTime;
    
    int32 CountOfPocketedBalls;
    
    
    UBaseBackendInfo* backend_data;
    
    cool_json_t CreateConsolidationJsonForPlayer(FString user_id);
    
public:
    uint8 FirstBallIndex;

    uint8 BallCountInGame;
protected:
    
    float GetCurrentTotalInGameTime();
    
    FString consolidation_event_name;
    cool_json_t consolidation_json_data;

public:	
    APoolPlayerControllerBase* playingPcs[4];

	APoolPlayerControllerBase* PlayingPc;

	

	bool firstShot;

	FTimerHandle FirstPlayerPlayDelay;

	float PlayTime = 15.f;

	virtual void OnPlayerStageStart();
	virtual void OnPlayerStageEnd();
    virtual void OnBallsStopped();
	virtual void OnPlayTimeEnd();
    
    virtual void OnReactionTimeUpdate(float newRtime);

	virtual void ProcessBallPocketed(int num, bool first);
	virtual void ProcessBallKnockedOff(int num);
	virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog);

	virtual bool CanPlayerShoot(APoolPlayerControllerBase* player);

	virtual void NextShot(uint8 pc_num);
    

	virtual uint8 GetFirstPCPlayedNum();
	virtual void ShuffleAndArrangeBalls();
	virtual void OnGameStart();
	virtual void CheckRules();
	virtual void CheckRules_FirstShot();
	virtual void CheckRules_NotFirstShot();
	virtual void CheckRules_NotFirstShot_BlackPocketed();
	virtual void CheckRules_NotFirstShot_BlackNotPocketed();
	virtual void CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit();
	virtual void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit();
	virtual void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted();
	virtual void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted();
	virtual void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted();
    
	virtual void CheckRules_Finalize();
	virtual bool IsReadyToFinalize();
    virtual void ProcessBase64Replay(const FString& replay);
	virtual void GameResults();
	virtual void PlayerExited(FString exiting_user_id);
	virtual void UpdateTargetableBalls(APoolPlayerStateBase* PlayerState);
};
