// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PoolBaseWidget.generated.h"

/**
 * 
 */

class USefaPoolGameInstance;

UCLASS()
class SEFAPOOL_API UPoolBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    
    UPROPERTY(BlueprintReadOnly)
    USefaPoolGameInstance* gi;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool needsCaching = false;

    bool transient = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool hideAfterCache = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool hideOnLevelChange = true;
    
    UFUNCTION(BlueprintImplementableEvent)
    void StartCache();
    
    UFUNCTION(BlueprintImplementableEvent)
    void EndCache();
    
    UFUNCTION(BlueprintImplementableEvent)
    void ShowPrep();
    
    UFUNCTION(BlueprintImplementableEvent)
    void HidePrep();
    
    UFUNCTION(BlueprintCallable)
    void Hide();
    UFUNCTION(BlueprintCallable)
    void Show();
    
    virtual void OnLevelRemovedFromWorld(ULevel * InLevel,
                                         UWorld * InWorld) override;
    
};
