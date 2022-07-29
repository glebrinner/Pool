// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Shared/SoundEngine.h"
#include "BallInfo.generated.h"

class AObjectGatherer;

class APool8BallPlayerController;

USTRUCT(BlueprintType)
struct FBallsInfo
{
	GENERATED_BODY()
public:
	FBallsInfo()
	{
		BallPos.SetNum(22);
		BallRotation.SetNum(22);
	}
	UPROPERTY()
	TArray<FVector> BallPos;
	UPROPERTY()
	TArray<FRotator> BallRotation;
    UPROPERTY()
    float time;
	UPROPERTY()
		int32 game_time;
	friend FArchive& operator<<(FArchive& Ar, FBallsInfo*& MC) 
	{
		Ar << MC->time;
		Ar << MC->BallPos;
		Ar << MC->BallRotation;
		Ar << MC->game_time;
		return Ar;
	}
};

USTRUCT(BlueprintType)
struct FSoundInfo
{
    GENERATED_BODY()
public:
    UPROPERTY()
    FVector pos;
    UPROPERTY()
    float power;
    UPROPERTY()
    E_PoolSounds sound;
    UPROPERTY()
    float time;
};

UCLASS()
class SEFAPOOL_API ABallInfo : public AActor
{
	GENERATED_BODY()
	AObjectGatherer* objects;
public:	
	// Sets default values for this actor's properties
	ABallInfo();

protected:
	virtual void BeginPlay() override;

	uint8 bIsReplayMode : 1;

public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FBallsInfo> BallsInfoArray;
    TArray<FSoundInfo> SoundsInfoArray;
    int singlePosUpdates = 0;
	//bool IsAllInfoSended() { return IsInfoSent; }
	
	int Iterator = 0;
    int IteratorSounds = 0;

	UPROPERTY(BlueprintReadOnly)
	bool bShouldGetInfo = false;

	FTimerHandle sendToClientsTimer;
    
    UFUNCTION(NetMulticast, Reliable)
    void ClearClientSideData();
    
    void StartClientUpload();
    void StopClientUpload();

	void StopReplayUpload();

	void StartClientUploadReplay();
    
    void SendToClient_FromServer();
    
    UFUNCTION(NetMulticast, Reliable)
    void SendToClient(const TArray<FBallsInfo>& ballsInfo, const TArray<FSoundInfo>& soundsInfo);

	UFUNCTION()
	TArray<FBallsInfo> GetNewDataPack();
    UFUNCTION()
    TArray<FSoundInfo> GetNewSoundPack();

	// void BeginTimer();

	// UFUNCTION(Server,Reliable)
	// void ServerGetBallsInfoFromClient(const TArray<FBallsInfo>& Info,bool IsLastPartOfInfo);

	void ClearBallsInfoArray();

	TArray<FBallsInfo> GetBallInfoArray() { return BallsInfoArray;}
    
    void UpdateBallPosOnClients();
    void UpdateBallPosOnSpecificClient(APool8BallPlayerController* pc);
    
    UFUNCTION(NetMulticast, Reliable)
    void SingleBallPosUpdate(const FBallsInfo& pos);

	void SetIsReplayMode(bool NewState) { bIsReplayMode = NewState; }
    
    bool IsReplayMode() { return bIsReplayMode;}
    
};
