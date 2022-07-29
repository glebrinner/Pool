// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PoolBaseShopWidget.h"
#include "PoolUserWidgetBase.h"
#include "PoolShopWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolShopWidgetBase : public UPoolUserWidgetBase
{
    GENERATED_BODY()
    
    
public:
    UPROPERTY(BlueprintReadWrite)
    TArray<int32> cues;
    
    // Definition is found PoolShopWidgetBP
    UFUNCTION(BlueprintImplementableEvent, Category = "ShopConstruction")
    void Init();

    UFUNCTION(BlueprintImplementableEvent, Category = "ShopUpdating")
    void UpdateCueWidget(int num, int durability);

    UFUNCTION(BlueprintImplementableEvent, Category = "ShopUpdating")
    void UpdateChalkWidget(int num, int durability);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ShopUpdating")
    void ActivateCueByIndex(int idx);
    
    void SetCuesArray(TArray<int32> cues);
    void ActivateCue(int num);
    
};
