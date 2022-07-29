// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SefaPool/8Ball/BallInfo.h"
#include "GameFramework/Actor.h"
#include "ReplayVisualization.generated.h"

UCLASS()
class SEFAPOOL_API AReplayVisualization : public AActor
{
	GENERATED_BODY()
	
	class AObjectGatherer* objects;
        
public:	
	// Sets default values for this actor's properties
	AReplayVisualization();
	
	uint8 BallCountInGame;

	protected:

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
	public:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void PlayNextStage();

	UFUNCTION(BlueprintCallable)
	void PlayPreviousStage();

	void StartVisualize();

	void StopVisualize();

	bool IsShouldVisualize() { return bShouldVisualize; }

	class ABallInfo* BallInfo;
	
	ABallInfo* GetBallInfo() { return BallInfo; }

	void ClearIterator() { iterator = 0;}

	void SetShouldVisualize(bool NewState) { bShouldVisualize = NewState; }

	int GetIterator() { return iterator;}
    
	void VisualizeSnapshot(const FBallsInfo& pos);
	void VisualizeSnapshotNow(const FBallsInfo& pos);

	int32 GetCurrentStage(int32 CurrentFrame);
    
	void TimedVisualizeSnapshot();

	void ReinitToNextStage();
};
