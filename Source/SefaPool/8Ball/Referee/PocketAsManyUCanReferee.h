// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/RefereeContestBase.h"
#include "PocketAsManyUCanReferee.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APocketAsManyUCanReferee : public ARefereeContestBase
{
	GENERATED_BODY()
	uint8 CountOfPocketedBalls;

	uint8 BallsPocketedOnPreviousStage;
    
    TArray<uint8> newPocketedBalls;
    
    bool foul;

public:
	APocketAsManyUCanReferee();
protected:
	void BeginPlay() override;
public:
    
    virtual void ShuffleAndArrangeBalls() override;
    virtual void OnPlayerStageStart() override;
    virtual void ProcessBallPocketed(int num, bool first) override;
    virtual void ProcessBallKnockedOff(int num) override;
	// void ProcessBallKnockedOff(int num) override;
    virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
    virtual int ContestId() override;
};
