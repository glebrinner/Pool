// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/RefereeBase.h"
#include "SPReferee.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API ASPReferee : public ARefereeBase
{
	GENERATED_BODY()

protected:
	
public:
	void BeginPlay() override;
	virtual void ProcessBallPocketed(int num, bool first) override;
	virtual void ProcessBallKnockedOff(int num) override;
	virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
	void UpdateTargetableBalls(APoolPlayerStateBase* PlayerState) override;
    
    virtual void NextShot(uint8 pc_num) override;
};
