// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolUserWidgetBase.h"
#include "Messages_ID.h"
#include "PoolMessageDeliveryWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum E_MessageSeverity
{
    IGNORED   = 0 UMETA(DisplayName = "Ignored"),
    SECONDARY = 1 UMETA(DisplayName = "Secondary"),
    PRIMARY   = 2 UMETA(DisplayName = "Primary"),
    CHAT      = 3 UMETA(DisplayName = "Chat"),
};

UCLASS()
class SEFAPOOL_API UPoolMessageDeliveryWidget : public UPoolUserWidgetBase
{
	GENERATED_BODY()
    TQueue<uint16> secondaryMessages;
    TQueue<uint16> primaryMessages;
    TQueue<uint16> chatMessages;
    
    TMap<uint16, FString> MessageText;
    
public:
    UFUNCTION(BlueprintCallable)
    void Init();
    
    void EnqueueMessageForDisplay(E_MessageSeverity type, uint16 messageId);
    
    UFUNCTION(BlueprintCallable)
    bool PrimaryEmpty();
    
    UFUNCTION(BlueprintCallable)
    FString GetPrimaryMessage();
    
    UFUNCTION(BlueprintCallable)
    bool SecondaryEmpty();
    
    UFUNCTION(BlueprintCallable)
    FString GetSecondaryMessage();
    
    UFUNCTION(BlueprintCallable)
    bool ChatEmpty();
    
    UFUNCTION(BlueprintCallable)
    FString GetChatMessage();
    
};
