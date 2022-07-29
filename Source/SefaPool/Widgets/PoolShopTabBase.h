// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PoolShopItemWidgetBase.h"
#include "PoolShopTabBase.generated.h"

/**
 * 
 */

class UPoolBaseShopWidget;

UCLASS()
class SEFAPOOL_API UPoolShopTabBase : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab name")
    FString name;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab name")
    EConsumableType consumableType;
    
    UPROPERTY(BlueprintReadWrite)
    UPoolShopItemWidgetBase* active;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemClass")
    TSubclassOf<UPoolShopItemWidgetBase> itemWidgetClass;
    
    UPROPERTY(BlueprintReadWrite)
    TArray<UPoolShopItemWidgetBase*> items;
    
    int registeredItems = 0;
    
    void RegisterItem(UPoolShopItemWidgetBase* item);
    
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ShopUpdating")
    void ClickOnOwnedItem(UPoolShopItemWidgetBase* widget);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ShopUpdating")
    void ClickOnNotOwnedItem(UPoolShopItemWidgetBase* widget);
    
    UPROPERTY(BlueprintReadWrite)
    UPoolBaseShopWidget* parent;
    
    virtual void NativeConstruct();
    
    void Refresh(bool ignoreNotOwned);
    
    //UFUNCTION(BlueprintCallable, Category = "ShopUpdating")
    void ChangeActiveItemInTab(EConsumableType consType,
                          int num);
    
    void UpdateItemDurability(EConsumableType consType,
                              int num,
                              int durability);
    
    void RefreshItem(EConsumableType consType,
                     int num);
    
    
    
    int presetId = 0;
};
