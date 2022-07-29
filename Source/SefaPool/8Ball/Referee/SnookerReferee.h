// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MPRefereeBase.h"
#include "SefaPool/8Ball/Referee/RefereeBase.h"
#include "SnookerReferee.generated.h"

UENUM()
enum EBallNum 
{
	CueBall = 0 UMETA(DisplayName = "CueBall"),
	FirstRedBall UMETA(DisplayName="First Red Ball"),
	LastRedBall = 15 UMETA(DisplayName="Las Red Ball"),
	Yellow  UMETA(DisplayName="Yellow Ball"),
	Green  UMETA(DisplayName="Green Red Ball"),
	Brown UMETA(DisplayName="Brown Red Ball"),
	Blue UMETA(DisplayName="Blue Ball"),
	Pink UMETA(DisplayName="Pink Ball"),
	Black UMETA(DisplayName="Black Ball")
};

USTRUCT()
struct FColoredBallData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector2D BallPos;

	UPROPERTY()
	uint8 BallPoint;

	FColoredBallData(FVector2D BallPos, uint8 BallPoint)
	{
		this->BallPoint = BallPoint;
		this->BallPos = BallPos;
	}

	FColoredBallData()
	{
		BallPos = FVector2D::ZeroVector;
		BallPoint = 0;
	}
	
};

USTRUCT()
struct FPlayerStats
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Points;

	UPROPERTY()
	bool bPocketedRedBall = false;

	UPROPERTY()
	uint8 PocketedBallsInFrame = 0;

	void Clear()
	{
		bPocketedRedBall = false;
		PocketedBallsInFrame = 0;
	}
	
};

UENUM()
enum EPenalty
{
	CueBallPocketed,
	RedBallPocketAgain,
	ColoredBallPocketedInsteadRedBall,
	Foul
};



UCLASS()
class SEFAPOOL_API ASnookerReferee : public AMPRefereeBase
{
	GENERATED_BODY()
	TMap<uint8,FColoredBallData> BallData;
	FPlayerStats PlayerStats[2];
	// FColoredBallStats ColoredBallStats;
	int32 FramePoints;
	int32 FoulPoint;
	uint8 ChoosedColoredBall;
	TArray<FVector2D> ColoredBallsPos;
	TArray<uint8> PocketedBallInFrame;
	TArray<uint8> PocketedColoredBallInFrame;

public:

	ASnookerReferee();
	bool IsPlayerSnookered();
	bool IsAllRedBallsPocketed();
	bool IsAllColoredBallsPocketed();
	void ChangeBallPosition(uint8 BallNum, FVector2D Pos);
	bool IsPositionFree(uint8 BallNum);
	void ReturnColoredBallIntoGame(uint8 BallNum);
	FVector FindDirectionToNearestTableWalls(FVector BallPos);
	uint8 GetSightingColoredBallIndex();
	int32 GetBallCost(uint8 BallNum);
	void CheckSnookerRules();
	void BeginPlay() override;
	virtual void OnPlayTimeEnd() override;
	virtual void ProcessBallPocketed(int num, bool first) override;
	virtual void ProcessBallKnockedOff(int num) override;
	virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
	void ShuffleAndArrangeBalls() override;
	virtual bool CanPlayerShoot(APoolPlayerControllerBase* player) override;
	virtual void NextShot(uint8 pc_num) override;
	void OnGameStart() override;
	bool IsReadyToFinalize() override;
	void CheckRules_Finalize() override;
	void CheckRules() override;
	void OnPlayerStageStart() override;
	uint8 GetOtherPlayerID() { return shotInfo.currentPlayerNum ? false : true; }
	void UpdateTargetableBalls(APoolPlayerStateBase* PlayerState);
	

	
	
};
