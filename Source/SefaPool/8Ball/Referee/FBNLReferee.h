// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/RefereeContestBase.h"
#include "FBNLReferee.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API AFBNLReferee : public ARefereeContestBase
{
	GENERATED_BODY()

	uint8 BallsPocketedOnPreviousStage;

	bool IsCueBallInTriangle();
    
    bool foul;
    
    bool firstShot_nofoul;

public:
	AFBNLReferee();
protected:
	void BeginPlay() override;
	public:
    virtual void ShuffleAndArrangeBalls() override;
    virtual void OnPlayerStageStart() override;
	void UpdateTargetableBalls(APoolPlayerStateBase* PlayerState) override;
    virtual void OnPlayTimeEnd() override;
    virtual void ProcessBallPocketed(int num, bool first) override;
    virtual void ProcessBallKnockedOff(int num) override;
    virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
    virtual int ContestId() override;
};
