// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Shared/PoolGameModeBase.h"
#include "ReplayGameMode.generated.h"


UCLASS()
class SEFAPOOL_API AReplayGameMode : public APoolGameModeBase
{
	GENERATED_BODY()

	class AObjectGatherer* ObjectGatherer;
public:
	AReplayGameMode();

protected:
    virtual void StartGame();
    virtual void BeginPlay() override;

	class APoolPlayerControllerBase* PlayerPC;
    bool levelReady;
	
public:
	void PostLogin(APlayerController* NewPlayer) override;

	void InitializeReplayManager();
	
    virtual void PCReady(APoolPlayerControllerBase* NewPlayer);
    
    virtual void LevelReady() override;
    
    
};

/*

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/GameModeBase.h"
#include "../Shared/Ball.h"
#include "../Shared/Table.h"
#include "PoolSPPlayerController.h"
#include "../Shared/PoolGameModeBase.h"
#if UE_SERVER
#include "AgonesComponent.h"
#endif
#include "PoolSPGameMode.generated.h"


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
    UFUNCTION(BlueprintCallable)
    void LevelReady();

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
    
    
};
*/
