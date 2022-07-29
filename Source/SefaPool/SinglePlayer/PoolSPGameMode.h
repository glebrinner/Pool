// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/GameModeBase.h"
#if UE_SERVER
#include "AgonesComponent.h"
#endif
#include "../Shared/Ball.h"
#include "../Shared/Table.h"
#include "PoolSPPlayerController.h"
#include "../Shared/PoolGameModeBase.h"
#if UE_SERVER
#include "AgonesComponent.h"
#endif
#include "PoolSPGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APoolSPGameMode : public APoolGameModeBase
{
	GENERATED_BODY()
private:
#if UE_SERVER
    UAgonesComponent* AgonesSDK;
#endif
	APoolSPPlayerController* playingPc;
	bool levelReady;
    bool firstShot;
	AObjectGatherer* objects;
protected:
	virtual void StartGame();
	void BeginPlay() override;
public:
	APoolSPGameMode();
    virtual void PCReady(APoolPlayerControllerBase* NewPlayer);
	virtual void LevelReady() override;

	void NextShot(APoolSPPlayerController* NewPlayer);

    virtual void ProcessBallPocketed(int num, bool first);
    virtual void ProcessBallKnockedOff(int num);
    virtual void ProcessHitLogAndFinalize(const TArray<int>& hitLog1, const TArray<int>& hitLog2);
    
    virtual void PreLogin(const FString& Options,
                          const FString& Address,
                          const FUniqueNetIdRepl& UniqueId,
                          FString& ErrorMessage) override;
    virtual APlayerController* Login(UPlayer* NewPlayer,
                                     ENetRole InRemoteRole,
                                     const FString& Portal,
                                     const FString& Options,
                                     const FUniqueNetIdRepl& UniqueId,
                                     FString& ErrorMessage) override;
    virtual void PostLogin(APlayerController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;
    
    void ExitServer();
    
    
    UFUNCTION()
    void AllocateAgones(const FGameServerResponse& Response);

    UFUNCTION()
    void AgonesSuccess(const FEmptyResponse& Response);

    UFUNCTION()
    void AgonesError(const FAgonesError& Response);
    
};
