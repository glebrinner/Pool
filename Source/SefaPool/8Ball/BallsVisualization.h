// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SefaPool/8Ball/BallInfo.h"
#include "GameFramework/Actor.h"
#include "BallsVisualization.generated.h"

class ObjectGatherer;

UCLASS()
class SEFAPOOL_API ABallsVisualization : public AActor
{
	GENERATED_BODY()

    
    
public:	
	// Sets default values for this actor's properties
	ABallsVisualization();
    
    bool soundVizHasEnded = false;

	bool bAutoPlayReplay;
	
	uint8 BallCountInGame;

protected:
	UPROPERTY(BlueprintReadOnly)
	AObjectGatherer* objects;
	
	int32 CurrentStage;
	bool bShouldVisualize;

	float ReplayMovableZero;
    FTimerHandle snapshotDelay;
    FBallsInfo snapshot;

	UPROPERTY(BlueprintReadOnly)
	int iterator;
    
    
    UPROPERTY(BlueprintReadOnly)
    int soundIterator;
    
    float shotBeginTime;

	bool bPreviousViz;
public:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PlayNextStage();

	UFUNCTION(BlueprintCallable)
	void PlayPreviousStage();

	UFUNCTION(BlueprintCallable)
	void StartVisualize();

	void StopVisualize();

	bool IsVisualize() { return bShouldVisualize; }

	ABallInfo* BallInfo;
	
	ABallInfo* GetBallInfo() { return BallInfo; }

	void ClearIterator() { iterator = 0;}

	void SetShouldVisualize(bool NewState) { bShouldVisualize = NewState; }

	int GetIterator() { return iterator;}
    
    void VisualizeSnapshot(const FBallsInfo& pos);
    void VisualizeSnapshotNow(const FBallsInfo& pos);

	int32 GetCurrentStage(int32 CurrentFrame);
    
    void TimedVisualizeSnapshot();

	void ReinitToNextStage();

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentVisualizedFrame();

	void SetAutoPlayReplay(bool NewState) { bAutoPlayReplay = NewState; }

	UFUNCTION(BlueprintCallable)
	void VisualizeFromFrame(int32 Frame);

	UFUNCTION(BlueprintCallable)
	float GetReplayProgress();

	UFUNCTION(BlueprintCallable)
	int32 GetFrameFromProgress(float Progress);
	
	
};
