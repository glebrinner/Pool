#pragma once

#include "stdio.h"

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "../Shared/PoolHUD.h"
#include "AgonesComponent.h"
#include "Pool8BallPlayerController.h"
#include "Pool8BallPlayerState.h"
#include "../Shared/PoolPlayer.h"
#include "../Shared/PoolGameModeBase.h"
#include "Pool8BallGameMode.generated.h"

UENUM()
enum class E_ShotResult
{
    UNDETERMINED = 0,
    SHOT_RETAINED = 5,
    SHOT_LOST = 6,
    FOUL = 7,
    WIN = 8,
    LOSE = 9
};

struct CurrentShotDetails
{
    bool         openTable;
    bool         lowPocketed;
    bool         highPocketed;
    bool         cuePocketed;
    bool         cueKnocked;
    bool         blackPocketed;
    bool         foul;
    int8         firstPocketedBall;
    int8         currentPlayerNum;
    int8         cueBallFirstOtherBallHit;
    int16        numRailTouchesAfterFirstHit;
    E_ShotResult shotResult;
    bool         readyToFinalize;
    CurrentShotDetails();
    void Clear();
};



class AObjectGatherer;
class AMPRefereeBase;

UCLASS()
class SEFAPOOL_API APool8BallGameMode : public APoolGameModeBase
{
    GENERATED_BODY()
public:

//#if UE_SERVER
    UAgonesComponent* AgonesSDK;
//#endif
    uint8 registeredPlayers;
    UPROPERTY()
        APool8BallPlayerController* playingPcs[4];
    bool readyPCs[4];
    TArray<APool8BallPlayerController*> Spectators;
    bool firstShot;
    CurrentShotDetails shotInfo;
    bool logged_out[4];
    AObjectGatherer* objects;
    bool bIsTeamMode;

    bool gameHasStarted;
    uint8 CurrentActivePlayerNum;
    
    void SetSpecPawn();
    APool8BallGameMode();
    virtual void BeginPlay();
    virtual void Tick(float dt);
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
    virtual void PCReady(APoolPlayerControllerBase* player);

    void StartGame();

    void ExitServer();
    void EnableTeamMode();

    void NextShot(uint8 pc_num);
    void ReturnSpectatorToMenu();

    void SetActivePlayerForSpectator(uint8 ActivePlayerNum);

    void BroadcastOpponentStatus(APool8BallPlayerController* pc,
                                 int cueNum,
                                 FVector loc,
                                 FRotator rot,
                                 FVector cueLoc,
                                 FRotator cueRot);
    
    void StartingPhysicsSimulationForShot (APoolPlayerControllerBase* pc);
    
    
    UFUNCTION()
    void AllocateAgones(const FGameServerResponse& Response);

    UFUNCTION()
    void AgonesSuccess(const FEmptyResponse& Response);

    UFUNCTION()
    void AgonesError(const FAgonesError& Response);

};
