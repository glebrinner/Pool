// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CueComponent.h"
#include "../Widgets/PoolMessageDeliveryWidget.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "PoolPlayerStateBase.generated.h"




/**
 * 
 */

USTRUCT()
struct FMessageInfo
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY()
    TEnumAsByte<E_MessageSeverity> severity;
    UPROPERTY()
    uint16 messageId;
};

UCLASS()
class SEFAPOOL_API APoolPlayerStateBase : public APlayerState
{
	GENERATED_BODY()
    
    TArray<FMessageInfo> activeMessages;
    class AObjectGatherer* ObjectGatherer;
protected:
    TArray<uint8> TargetableBalls;
    EDifficultyLevels DiffLevel;
public:
    UPROPERTY(replicated)
    FCueInfo cueInfo;

	UPROPERTY(replicated)
	bool ballInHand;
    
    APoolPlayerStateBase();
    
    bool CanSelectCue(int num);
    bool SelectCue(int num);
    
    bool CanSelectChalk(int num);
    bool SelectChalk(int num);
    
    UFUNCTION(NetMulticast, Reliable)
    void InitClient(const TArray<int32>& cue_durabilities,
                    const TArray<int32>& chalk_durabilities,
                    int current_cue,
                    int current_chalk,
                    EDifficultyLevels difficulty);

    void InitClient_Implementation(const TArray<int32>& cue_durabilities,
                                   const TArray<int32>& chalk_durabilities,
                                   int current_cue,
                                   int current_chalk,
                                   EDifficultyLevels difficulty); // Do not need to declare this. Use for inline compile in VS
    
    void ConsumablesReady();
    
    virtual void UpdateClient();
    
    virtual void GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const;
    
    virtual bool CanTargetThisBall(int n);

    
    UFUNCTION(BlueprintCallable)
    FItemDurability& GetItemDurability();
    
    UFUNCTION(BlueprintCallable)
    int GetCurrentCue();

    UFUNCTION(BlueprintCallable)
    int GetCurrentChalk();
    
    UFUNCTION(Client, Reliable)
    void TransferItemDurabilityToGameInstance();
    UFUNCTION(Client,Reliable)
    virtual void ClientSetTargetableBalls(const TArray<uint8> &ArrayOfTargetableBall);
    
    void RegisterMessage(E_MessageSeverity severity, uint16 messageId);
    
    void SendImmediateMessage(E_MessageSeverity severity, uint16 messageId);
    void ClearMessages();
    void Server_DeployMessages();
    
    UFUNCTION(Client, Reliable)
    void Client_DeployMessages(const TArray<FMessageInfo>& mess);

    UFUNCTION(Client,Reliable)
    void Client_ForceUpdateTargetableBalls();

    void BeginPlay() override;

public:
    UPROPERTY(replicated, BlueprintReadOnly)
    FString user_id;
    UPROPERTY(replicated, BlueprintReadOnly)
    FString username;
    UPROPERTY(replicated)
    FItemDurability item_durability_;
	
};
