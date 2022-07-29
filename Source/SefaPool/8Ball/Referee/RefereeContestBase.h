// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/8Ball/Referee/RefereeBase.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPool/Server/PoolServerGameInstance.h"
#include "PoolContestPlayerState.h"
#include "RefereeContestBase.generated.h"

/**
 * 
 */



UCLASS()
class SEFAPOOL_API ARefereeContestBase : public ARefereeBase
{
	GENERATED_BODY()
    
    UBackendContestInfo* info;
    
    int division;
    int currentCompetitionId;
    int fee;
    
    
public:
    
    virtual void BeginPlay() override;
    
    virtual int ContestId();
    virtual void GameResults();
    virtual void ProcessBase64Replay(const FString& replay);
    void UpdateContestPS();
    
    virtual void OnReactionTimeUpdate(float newRtime) override;
    
    virtual void OnBallsStopped();
    virtual void OnPlayTimeEnd();
    
    virtual void UpdateTargetableBalls(APoolPlayerStateBase* PlayerState);
    
    virtual void OnGameStart();
};
