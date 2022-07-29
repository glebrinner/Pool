// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainSettingsWidget.h"
#include "Wrappers/BaseSettingsWrapper.h"
#include "SettingsTabWidget.generated.h"

/**
 * 
 */

UCLASS()
class SEFAPOOL_API USettingsTabWidget : public UUserWidget
{
	GENERATED_BODY()
    
    void GenerateWidgetForItem(const FWidgetPrepInfo& wpi);
    
public:
    
    UPROPERTY(meta = (BindWidget))
    class UScrollBox* MainScrollBox;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab name")
    FString name;
    
    UPROPERTY()
    TArray<FWidgetPrepInfo> itemInfos;
    
    UPROPERTY(BlueprintReadOnly)
    TArray<UBaseSettingsWrapper*> itemWidgets;
    
    void RegisterItem(FWidgetPrepInfo&& wpi);
    
    UPROPERTY(BlueprintReadOnly)
    UMainSettingsWidget* parent;
    
    virtual void NativeConstruct();
    UFUNCTION(BlueprintCallable)
    void Reset();
    UFUNCTION(BlueprintImplementableEvent)
    void Activate();
    UFUNCTION(BlueprintImplementableEvent)
    void Deactivate();
    void Refresh();
};
