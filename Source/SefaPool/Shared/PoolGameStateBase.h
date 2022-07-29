

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/GameStateBase.h"
#include "Ball.h"
#include "Table.h"
#include "SharedDefs.h"
#include "PoolPlayerControllerBase.h"
#include <functional>

#include "SefaPool/8Ball/Pool8BallPlayerState.h"
#include "PoolGameStateBase.generated.h"

/**
 *
 */
UENUM()
enum ShotRuleset
{
    RULES_ANYWHERE = 0,
    RULES_BEHIND_HEADSTRING = 1,
    RULES_KITCHEN = 2,
    RULES_CIRCLE_OF_BALLS = 3
};

class AObjectGatherer;
UCLASS()
class SEFAPOOL_API APoolGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
    
    bool alreadyBegun;
    ShotRuleset rules;
    
private:
	APhysicsSetup* physicsSetup;
public:
    AObjectGatherer* objects;
private:
	bool bReadyBallsAndTable;
	FVector pocketedStorage;
public:
	UPROPERTY(Replicated)
	APool8BallPlayerState* FirstPlayerPS;

	UPROPERTY(Replicated)
	APool8BallPlayerState* SecondPlayerPS;

	UPROPERTY(ReplicatedUsing=OnRep_OnGameTimeUpdate)
	int32 GameTime;

	void ExecuteGameTime();
	
	void UpdateGameTime();

	void StopGameTime();

	UFUNCTION()
	void OnRep_OnGameTimeUpdate();

	FTimerHandle GameTime_Timer;

	UPROPERTY(ReplicatedUsing=OnRep_OnSpecCountChanged, BlueprintReadOnly)
	int32 SpectatorCount;

	bool ballsInGame[22];
public:
	APoolGameStateBase();

	UFUNCTION(NetMulticast,Unreliable)
	void Multicast_UpdateUserBarWidget();

	UFUNCTION()
	void OnRep_OnSpecCountChanged();
	
	virtual void BeginPlay();

	FVector GetCueBallDefaultLocationOnTable();
	FVector GetFoulBallDefaultReturnLocationOnTable();

	FVector GetPocketedBallStorageLocation(int num);

	void ReturnCueBall();
	void ReturnFoulBall(int num);
    void StorePocketedBall(int num);

	void ShuffleAndArrangeBalls();
    
    std::function<bool(FVector2D)> GetRulesetFunction ();
    
    void SetRuleset(ShotRuleset rules_ = ShotRuleset::RULES_ANYWHERE);
    UFUNCTION(NetMulticast, Reliable)
    void UploadRules(ShotRuleset rules_);


};



#define GAME_STATE_SYNC(gs, function) \
APoolGameStateBase* gs = GetWorld()->GetGameState<APoolGameStateBase>(); \
if (gs) gs->function; \
else ERROR_DEBUG_PRINT_FUNC_INFO("Game state not found")
