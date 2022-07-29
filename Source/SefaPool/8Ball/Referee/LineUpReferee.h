// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/RefereeContestBase.h"
#include "LineUpReferee.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API ALineUpReferee : public ARefereeContestBase
{
	GENERATED_BODY()

	FVector2D CueBallPosition;

	float DistanceBetweenBallsInTableCoords;

	float DistanceBeetweenBallsInUnrealUnits;

	FVector2D LineUpStartPosition;
    FVector2D LineUpEndPosition;

	

	uint8 BallsPocketedOnPreviousStage;
    
    bool foul;

public:
	ALineUpReferee();

	void Initialize();
protected:
	void BeginPlay() override;

	void ShuffleAndArrangeBalls() override;

	void PlaceBallOnTablePosition(FVector2D TablePosition);
	
public:
	virtual void OnPlayerStageStart() override;
    virtual void OnPlayTimeEnd() override;
    virtual void NextShot(uint8 pc_num) override;
    virtual void ProcessBallPocketed(int num, bool first) override;
    virtual void ProcessBallKnockedOff(int num) override;
    virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
    virtual int ContestId() override;
    
	
};
