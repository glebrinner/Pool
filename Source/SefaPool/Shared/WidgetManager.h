// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "SefaPool/Widgets/PoolBaseWidget.h"
#include "WidgetManager.generated.h"

class USefaPoolGameInstance;

USTRUCT()
struct FWidgetInfo {
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY()
    TSubclassOf<UPoolBaseWidget> widgetClass;
    
    UPROPERTY()
    UPoolBaseWidget* widget;
    
    UPROPERTY()
    int zOrder;
};

UCLASS()
class SEFAPOOL_API UWidgetManager : public UObject
{
	GENERATED_BODY()
    
    UPROPERTY()
    TMap<FString, FWidgetInfo> widgets;
    
public:
    
    UFUNCTION(BlueprintCallable)
    bool AddWidget(FString name,
                   TSubclassOf<UPoolBaseWidget> widgetClass,
                   int zOrder = 0);
    
    UFUNCTION(BlueprintCallable)
    void InitAndStartCaching(USefaPoolGameInstance* gi);
    
    UFUNCTION(BlueprintCallable)
    void EndCaching();
    
    UFUNCTION(BlueprintCallable)
    bool ShowWidget(FString name);
    
    UFUNCTION(BlueprintCallable)
    bool HideWidget(FString name);
    
    UFUNCTION(BlueprintCallable)
    UPoolBaseWidget* GetWidget(FString name);
    
    UFUNCTION(BlueprintCallable)
    UPoolBaseWidget* CreateTransientWidget(TSubclassOf<UPoolBaseWidget> widgetClass, UWorld* world, FString name);
    UFUNCTION(BlueprintCallable)
    void HideOnOpenLevel();
};
