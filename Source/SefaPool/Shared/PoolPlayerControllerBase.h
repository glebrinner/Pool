// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/PlayerController.h"
#include "PoolPlayer.h"
#include "Components/WidgetComponent.h"
#include "SefaPool/8Ball/BallsVisualization.h"
#include "PoolGameFlow.h"
#include "PoolInputPlayerController.h"
#include "Table.h"
#include "../Widgets/LoadingWidget.h"
#include "PoolPlayerStateBase.h"
#include "../Widgets/PoolShopWidgetBase.h"
#include "../Widgets/PoolMessageDeliveryWidget.h"
#include "PoolPlayerControllerBase.generated.h"

/**
 * 
 */
class AObjectGatherer;
UCLASS()
class SEFAPOOL_API APoolPlayerControllerBase : public APoolInputPlayerController
{
	GENERATED_BODY()
public:
    AObjectGatherer* objects;
protected:
	APoolPlayer* playingPawn;
	bool bIsBallSeted;
	bool aiming;
	bool spectating;
	FVector2D touchPrevPos;
    FVector2D pinchPrevPos;
    float pinchPrevSize;
	bool touching;
    bool pinching;
    FTimerHandle pendingStateChangeTimer;
    FTimerHandle initTimer;
    GameFlowState_t nextState;
    bool ballHasBeenMoved;
    bool cuesReady;
    ETouchIndex::Type caughtFinger;
    ETouchIndex::Type caughtFinger2;
    float lastPinchStartTime;
    float lastTouchStartTime;
    float shotStartTime;
    bool usingAngle;
    TArray<ETouchIndex::Type> fingersRelayedToHUD;
    bool actuallyStartAimingOnce;
    bool playerIsChangingPower;
    bool inPauseMenu;
    
    
	UPROPERTY(Replicated)
	uint8 bSpectator : 1;
public:
	APoolPlayerControllerBase();


	virtual void BeginPlay() override;
	virtual void Tick(float dt) override;
	UFUNCTION(Client, Reliable)
	void ClientSetViewToTable();
	void ClientSetViewToTable_Implementation();
    
    void TimedServerPCReady();
	UFUNCTION(Server, Reliable)
	void ServerPCReady();
	virtual void ServerPCReady_Implementation();

	UFUNCTION(Client, Reliable)
    void ClientStartPlay();
	void ClientStartPlay_Implementation();

	UFUNCTION(Server,Reliable)
	void Server_CalculateCurrentIdleDelay();
    
    void ProceedToNextStageIfVisualizationHasEnded();
    
    void ActuallyStartAiming();

	void ClientEndPlay(ABall* targetBall, FVector targetVec, float shotPower);
	void ShotEnded();

	virtual void SetupInputComponent() override;

	UFUNCTION(BlueprintCallable)
	void OnPlay();

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void ServerOnPlay();
    
	void OnTouchPressed(ETouchIndex::Type FingerIndex, FVector Location);
	void OnTouchReleased(ETouchIndex::Type FingerIndex, FVector Location);
    void DoublePinchTap();
    void Tap();
	
	UFUNCTION(BlueprintCallable)
	void SetPower(float value);
	
	UFUNCTION(BlueprintCallable)
	void SetAimDisplacement(FVector2D disp);
	UFUNCTION(BlueprintCallable)
	FVector2D GetAimDisplacement();
    
    void EffectMovementHasBegun();
    void EffectMovementHasEnded();
    
	UFUNCTION(BlueprintCallable)
	virtual void CurrentFrameNextBallsToHit(TArray<int32>& arr);
    UFUNCTION(BlueprintCallable)
    virtual UCueComponent* GetCueComponent();
    UFUNCTION(BlueprintCallable)
    virtual void Switch3DCamera();
    UFUNCTION(BlueprintCallable)
    virtual void Stop3DCamera();
    UFUNCTION(BlueprintCallable)
    virtual void Switch2DCamera();
    UFUNCTION(BlueprintCallable)
    virtual void SwitchBallInHand();
    UFUNCTION(BlueprintCallable)
    virtual void ApplyChalk();
    UFUNCTION(BlueprintCallable)
    virtual int ShotsBeforeRechalking();
    UFUNCTION(BlueprintCallable)
    virtual int GetCurrentCue();
    UFUNCTION(BlueprintCallable)
    virtual void ChangeCue(int num);
    UFUNCTION(BlueprintCallable)
    virtual void ChangeChalk(int num);
    UFUNCTION(BlueprintCallable)
    virtual class UPoolPowerBarChalkWidget* GetGameWidget();
    UFUNCTION(BlueprintCallable)
    virtual float GetCueAngle();
    UFUNCTION(BlueprintCallable)
    virtual void ElevateCue(float angle);
    UFUNCTION(BlueprintCallable)
    virtual bool IsAngleChanging();
    UFUNCTION(BlueprintCallable)
    virtual void PauseMenuEntered();
    UFUNCTION(BlueprintCallable)
    virtual void PauseMenuExited();
    
    UFUNCTION(Server, Reliable)
    void ApplyChalk_Server();
    UFUNCTION(Server, Reliable)
    void ChangeCue_Server(int num);
    UFUNCTION(Server, Reliable)
    void ChangeChalk_Server(int num);
    UFUNCTION(Server, Reliable)
    void RechargeCue_Server(int num);
    UFUNCTION(Server, Reliable)
    void RechargeChalk_Server(int num);

    
    void SwitchAngle();
    void StartAngleChange();
    void EndAngleChange();
    void PowerChangeStarted();
    void PowerChangeEnded();
    
    void InitiateShot(FVector target, FVector2D offset, float power);
    
    UFUNCTION(Server, Reliable)
    void InitiateShot_Server(FVector target, FVector2D offset, float power, bool usingAngle_);
    UFUNCTION(Server, Reliable)
    void TransferBallInHandMovement_Server(FVector target);
    /*
    UFUNCTION(Server,Reliable)
    void Server_OnPlayTimeEnded();
    */
	UFUNCTION(Client, Reliable)
	void ClientTravelToLevel(const FName& level, const FString& widget, const FString& options="");
	void ClientTravelToLevel_Implementation(const FName& level, const FString& widget, const FString& options="");
    void TimedTravelToLevel(FName level, FString widget, FString options="");
    
    virtual bool IsReady();
    
    virtual void ActivateFoulIfNeeded();
    virtual void UpdateUserBarWidget();
    
    
    virtual void BallInHandMoveBallIncremental(FVector2D vec);
    
    void Adjust3DCam(FVector2D amount);
    void InitShop();
    
	UFUNCTION(Server,Reliable)
	void ServerOnPlayerStageEnd();
    
    void ShowShop();
    
    UFUNCTION(Client, Reliable)
    void UpdateShop(EConsumableType type, int num, int durability);

    UFUNCTION(Client, Reliable, BlueprintCallable)
    void ActivateCue(int idx);

    
    UFUNCTION(BlueprintCallable)
    void ChangeCloth(E_TableClothColors color);
    
    UFUNCTION(BlueprintCallable)
    void ChangeClothColor(FLinearColor color, float adj);
    
    UFUNCTION(Client, Reliable)
    void ShotTimeHasEnded();
	
    virtual void CueTimeHasEnded_Body();
    
    UFUNCTION(BlueprintCallable)
    float GetShotTimeLeft();
    
    UFUNCTION(BlueprintCallable)
    float GetTotalShotTime();
    
    
    void DisplayMessage(E_MessageSeverity sev, uint16 messageId);

	UFUNCTION(Client,Reliable)
	void ClientDrawDebug(FVector DrawStart, FVector DrawEnd = FVector::ZeroVector);

	UFUNCTION(Client,Reliable)
	void ClientDrawDebugLine(FVector TraceStart,FVector TraceEnd);
    UFUNCTION(Client,Reliable)
    void ClientSetBallManagerSettings(uint8 FirstBallIndex,uint8 BallCountInGame);

	void PossessBySpectatorClass();

	void SetIsSpectator() { bSpectator = true; }

	bool IsSpectator() { return bSpectator; }
    
    
    // Used by server only
    FString Options_;
};
