// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RefereeContestBase.h"
#include "CircleOfBallsReferee.generated.h"

UCLASS()
class SEFAPOOL_API ACircleOfBallsReferee : public ARefereeContestBase
{
	GENERATED_BODY()
    
    uint8 BallsPocketedOnPreviousStage;
    
    bool foul;
public:	
	ACircleOfBallsReferee();

protected:
	
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	float CircleRadiusInTableCoords;

	UPROPERTY(EditDefaultsOnly)
	FVector2D CircleCenter;

	void Test();
private:
	float CircleRadiusInUnrealUnits;

	FVector CircleCenterIn3D;

	bool bMakeRandomCircleAngle;

	bool IsPocketBallInCircle();

	//void OnPlayerStageStart() override;

public:
    virtual void NextShot(uint8 pc_num);
    
    virtual void OnPlayTimeEnd() override;
    virtual void ProcessBallPocketed(int num, bool first) override;

    virtual void ProcessBallKnockedOff(int num) override;
	
    virtual void ShuffleAndArrangeBalls() override;

    virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
	
    virtual void PlaceBallOnTablePosition(FVector2D TablePosition);
    virtual int ContestId() override;
};


