// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TournamentPlayerState.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "SefaPool/Shared/PlayerVarificationGameMode.h"
#include "TournamentPlayerController.h"
#include "TimerManager.h"

#if UE_SERVER
#include "RedisObject.h"
#endif

#include "SefaPool/Server/GsAsyncQueueAllocator.h"
#include "Tournament.generated.h"



class ABaseTournamentGameMode;

USTRUCT()
struct FMatchInfo {
	GENERATED_BODY()

	UPROPERTY()
	FString win_user_id;

	UPROPERTY()
	int balls_pocketed[2] = { -1, 1 };

	UPROPERTY()
	int scores[2] = { -1, -1 };
};

USTRUCT()
struct FPlayerInfo_TournamentMatch {
	GENERATED_BODY()

	UPROPERTY()
		FString participating_user_id;
	UPROPERTY()
		bool ready_user_id = true;
	UPROPERTY()
		bool accepted_user_id = false;
};

UCLASS()
class SEFAPOOL_API UTournamentMatch : public UObject {
	GENERATED_BODY()
    
public:
	UTournamentMatch() = default;
	void Init(UTournament* tournament, int match_id, int stage);

	bool AddPlayer(FString user_id);

	void RegisterIncomingPlayer(APlayerController* pc);

	bool IsReady();

	bool StartIfReady();
	virtual void BeginDestroy() override;

	FTimerHandle FirstPlayerToConnectTimer;
	FTimerHandle SecondPlayerToConnectTimer;

	UFUNCTION()
	void FinishFirstPlayerToConnect();
	UFUNCTION()
	void FinishSecondPlayerToConnect();

	UPROPERTY()
	int match_id_ = -1;
private:
	UTournament* tournament_;
	FMatchInfo info;
	TArray<APlayerController*> pcs;
	UPROPERTY()
	int stage_ = -1;

	UPROPERTY()
	TArray<FPlayerInfo_TournamentMatch> PlayersInfo;

	//UPROPERTY()
	//TArray<FString> participating_user_id = {};
	//UPROPERTY()
	//TArray<bool> ready_user_id = { false, false };
	//UPROPERTY()
	//TArray<bool> accepted_user_id = { false, false };
};

UCLASS()
class SEFAPOOL_API UTournament : public UObject
{
	GENERATED_BODY()
    
public:
    UPROPERTY()
    UGsAsyncQueueAllocator* gs_allocate;

	void Init(ABaseTournamentGameMode* gm, int tourn_id_, EDifficultyLevels difficulty_, int size);

	bool Start();

	void RegisterIncomingPlayer(APlayerController* pc);

	UFUNCTION()
	void ProcessingMatches(const FString& channel, const FString& message);
    
    virtual void BeginDestroy() override;
    UFUNCTION()
    void RedisPing();

public:
	UPROPERTY()
	ABaseTournamentGameMode* gm_;

#if UE_SERVER
	URedisObject* redis;
#endif
	UPROPERTY()
	TArray<UTournamentMatch*> tree;
	UPROPERTY()
	TMap<FString, APlayerController*> userid_to_pc;
	UPROPERTY()
	TMap<FString, UTournamentMatch*> userid_to_match;
    UPROPERTY()
    TMap<FString, FString> userid_to_username;
	UPROPERTY()
	TMap<int, int> win_amounts;

    
	int tourn_id = 1;
	EDifficultyLevels difficulty = EDifficultyLevels::DIFFICULTY_LEVEL_6;
	int size = 4;
	bool has_started = false;
	bool has_ended = false;
    
    cool_json_t tournament_end_json;
    
    FTimerHandle pingTimer;
};
