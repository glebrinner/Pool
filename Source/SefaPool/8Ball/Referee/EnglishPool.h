// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/MPRefereeBase.h"
#include "EnglishPool.generated.h"


USTRUCT()
struct FPlayersStates
{
	GENERATED_BODY()

	FPlayersStates() : LowPocketed(false),HighPocketed(false),BlackPocketed(false),bDoubleShot(false),
	bIsFouled(false), bIsSnookered(false) {}
	
	bool LowPocketed;

	bool HighPocketed;

	bool BlackPocketed;
	
	bool bDoubleShot;

	bool bIsFouled;

	bool bIsSnookered;

	bool IsSightBallsSet() { return LowPocketed || HighPocketed || BlackPocketed; }

	bool IsPlayerHasDoubleShot() { return bDoubleShot;}

	
};

UCLASS()
class SEFAPOOL_API AEnglishPool : public AMPRefereeBase
{
	GENERATED_BODY()

	AEnglishPool();

	FPlayersStates PlayersStates[2];

	TArray<uint8> StagePocketedBalls;
public:
	bool IsSightBallPocketed(int32 BallNum);
	bool IsAllRedAdnYellowBallsPocketed();
	bool IsAllYellowBallsPocketed();
	bool IsAllRedBallsPocketed();
	virtual void OnPlayTimeEnd() override;
	virtual void ProcessBallPocketed(int num, bool first) override;
	virtual void ProcessBallKnockedOff(int num) override;
	virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
	virtual void OnPlayerStageStart() override;
	virtual bool CanPlayerShoot(APoolPlayerControllerBase* player) override;
	virtual void NextShot(uint8 pc_num) override;
	void OnGameStart() override;
	void CheckRules() override;
	virtual void CheckRules_FirstShot_(uint8 BallNum);
	void UpdateTargetableBalls(APoolPlayerStateBase* PlayerState);
	bool IsPlayerSnookered();
	
	// void CheckRules_FirstShot() override;
	/* void CheckRules_NotFirstShot() override;
	void CheckRules_NotFirstShot_BlackPocketed() override;
	void CheckRules_NotFirstShot_BlackNotPocketed() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted() override;*/
	// bool IsReadyToFinalize() override;
	void CheckRules_Finalize() override;

	void BeginPlay() override;
};
