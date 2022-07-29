// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../Shared/PoolPlayerControllerBase.h"
#include "Pool8BallPlayerState.h"
#include "Pool8BallPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APool8BallPlayerController : public APoolPlayerControllerBase
{
	GENERATED_BODY()
public:
	virtual void ServerPCReady_Implementation() override;
	virtual void CurrentFrameNextBallsToHit(TArray<int32>& arr) override;
    virtual void ActivateFoulIfNeeded() override;
    virtual void UpdateUserBarWidget() override;
    
    UFUNCTION(Server, Unreliable)
    void UploadOpponentInfo(int cueNum, FVector loc, FRotator rot, FVector cueLoc, FRotator cueRot);
    
    UFUNCTION(Client, Reliable)
    void UpdateOpponentStatus(int cueNum, FVector loc, FRotator rot, FVector cueLoc, FRotator cueRot);
    
    UFUNCTION(Client, Reliable)
    void StopOpponentUpdate();
    UFUNCTION(Client, Reliable)
    void UpdateClientBallPos(const FBallsInfo& pos);

	UFUNCTION(Client,Reliable)
	void Client_UpdateActivePlayer(uint8 ActivePlayer);
    
    
    virtual void CueTimeHasEnded_Body() override;
    UFUNCTION(BlueprintCallable)
    APool8BallPlayerState* GetOpponentPlayerState();
    
    UFUNCTION(Client, Reliable)
    void SetUsernames(const FString& left, const FString& right);

};
