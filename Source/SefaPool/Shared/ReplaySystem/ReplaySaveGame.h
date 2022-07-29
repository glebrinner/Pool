// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SefaPool/8Ball/BallInfo.h"
#include "GameFramework/SaveGame.h"
#include "ReplaySaveGame.generated.h"


enum class ERecordedGameMode : uint8;

USTRUCT(BlueprintType)
struct FStageData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FBallsInfo> BallInfoArray;

	UPROPERTY()
	uint8 ActivePlayerNum;

};

USTRUCT()
struct FStageSoundInfo
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FSoundInfo> SoundInfos;
};

UCLASS()
class SEFAPOOL_API UReplaySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UReplaySaveGame();
	
	UPROPERTY(SaveGame)
	TArray<FStageData> FrameBallArray;

	UPROPERTY(SaveGame)
	uint8 GameMode;

	UPROPERTY(EditDefaultsOnly)
	TArray<FStageSoundInfo> StageSoundInfo;
	
	UPROPERTY(SaveGame)
	TArray<int32> StagesEnd;

	UPROPERTY(SaveGame)
	FString FirstPlayerNickName;

	UPROPERTY(SaveGame)
	FString SecondPlayerNickName;

	void AddSoundInfo(TArray<FSoundInfo> SoundInfos);

	void AddStageDatas(TArray<FStageData> StageDatas);
	

	// todo add player id, match result etc;
};
