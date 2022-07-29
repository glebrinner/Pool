// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/RefereeContestBase.h"
#include "SpeedPoolReferee.generated.h"

UCLASS()
class SEFAPOOL_API ASpeedPoolReferee : public ARefereeContestBase
{
	GENERATED_BODY()
	uint8 CueBallFoulCount;
    uint8 prevCueBallFoulCount;

	float PenaltyTime = 5.f;

protected:
	
	void BeginPlay() override;
public:
    
    virtual void OnPlayTimeEnd() override;
    
    virtual void ProcessBallPocketed(int num, bool first) override;
    virtual void ProcessBallKnockedOff(int num) override;
    virtual void OnPlayerStageStart() override;
    virtual void OnPlayerStageEnd() override;
    virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
    virtual int ContestId() override;
	
};
