// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/MPRefereeBase.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "Pool8BallReferee.generated.h"

/**
 * 
 */
USTRUCT()
struct FCurrentShotCastedPlayers
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY()
    APool8BallPlayerController* pc;
    UPROPERTY()
    APool8BallPlayerState* ps;
    UPROPERTY()
    APool8BallPlayerController* other_pc;
    UPROPERTY()
    APool8BallPlayerState* other_ps;
};

UCLASS()
class SEFAPOOL_API APool8BallReferee : public AMPRefereeBase
{
	GENERATED_BODY()
    
    FCurrentShotCastedPlayers currentPlayers;
protected:
    bool is_firm_break = false;
    bool is_firm_replay = false;
    
public:
	APool8BallReferee();


	void BeginPlay() override;
	virtual void OnPlayTimeEnd() override;
	virtual void ProcessBallPocketed(int num, bool first) override;
	virtual void ProcessBallKnockedOff(int num) override;
	virtual void ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog) override;
    virtual void OnReactionTimeUpdate(float newRtime) override;
	virtual bool CanPlayerShoot(APoolPlayerControllerBase* player) override;
	virtual void NextShot(uint8 pc_num) override;
	void OnGameStart() override;
	void CheckRules() override;
	void CheckRules_FirstShot() override;
	void CheckRules_NotFirstShot() override;
	void CheckRules_NotFirstShot_BlackPocketed() override;
	void CheckRules_NotFirstShot_BlackNotPocketed() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_OpenTableWithValidHit() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_LowBallsTargeted() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_HighBallsTargeted() override;
	void CheckRules_NotFirstShot_BlackNotPocketed_NotOpenTableWithValidHit_BlackBallTargeted() override;
	bool IsReadyToFinalize() override;
	void CheckRules_Finalize() override;
	void GameResults() override;
    void ProcessBase64Replay(const FString& replay) override;
	void RequestConsolidateMatch(FString win_user_id, FString lose_user_id);
	void PlayerExited(FString exiting_user_id) override;

public:
	FName win_game_level = TEXT("Authentication");
	FName lose_game_level = TEXT("Authentication");
	FString win_game_ip;
	int win_game_port;
	FString win_game_widget = TEXT("WinScreen");
	FString lose_game_widget = TEXT("LoseScreen");
};
