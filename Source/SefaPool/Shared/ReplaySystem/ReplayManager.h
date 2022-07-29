// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"
#include "SefaPool/Shared/PoolPlayerControllerBase.h"
#include "GameFramework/Actor.h"
#include "ReplayManager.generated.h"


UENUM()
enum class ERecordedGameMode : uint8
{
	CircleOfBalls = 0,
	FBNL,
	LineUp,
	PocketAsManyUCan,
	Pool8Ball,
	Snooker,
	SpeedPool,
	EnglishPool
};

UCLASS()
class SEFAPOOL_API AReplayManager : public AActor
{
	GENERATED_BODY()

	bool bIsMatchSaved;

	class AObjectGatherer* ObjectGatherer;
public:	
	AReplayManager();

protected:
	UPROPERTY(BlueprintReadOnly)
	TArray<FStageData> FrameBallArray;

	UPROPERTY()
	TArray<FSoundInfo> SoundInfos;

	TArray<FStageSoundInfo> StageSoundInfos;

	TArray<int32> StagesEnd;

	ERecordedGameMode GameMode;

	virtual void BeginPlay() override;

	int32 ReplayFrameCount;

	FTimerHandle NickName_Timer;

	void TrySavePlayerNickNames();

public:
	FString FirstPlayerNickName;

	FString SecondPlayerNickName;
	
	TArray<int32> GetStagesEnd() { return StagesEnd; }
	void AppendStageVisualization(FStageData StageDatas);

	void AppendSoundInfo(FSoundInfo SoundInfo);

	void InitializeGameMode(ERecordedGameMode GMType);

    void OnGameEnd(TArray<uint8>& replayData);

	void LoadMatchReplay();

	void PrepareToPlayReplay(APoolPlayerControllerBase* PlayerPC);

	void InitializePlayerData(FString FirstPlayer, FString SecondPlayer = "");

	UFUNCTION(NetMulticast,Reliable)
	void Multicast_ShareReplayData(const TArray<int32>& StagesEndArray);
	
	virtual void Tick(float DeltaTime) override;

	// UFUNCTION(BlueprintCallable)
	// float GetReplayProgress(int32 CurrentFrame);
	//
	// UFUNCTION(BlueprintCallable)
	// int32 GetFrameFromProgress(float Progress);

	TArray<FStageData> & GetFrameBallArray() { return FrameBallArray;}

	TArray<FStageSoundInfo> & GetSoundInfo() { return StageSoundInfos; }
    
    void PlayReplay();

};
