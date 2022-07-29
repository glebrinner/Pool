// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Shared/CueComponent.h"
#include "../Shared/SharedDefs.h"
#include "PoolBaseWidget.h"

#include "Blueprint/UserWidget.h"
#include "PoolShopTabBase.h"
#include "PoolBaseShopWidget.generated.h"



UCLASS()
class SEFAPOOL_API UPoolBaseShopWidget : public UPoolBaseWidget
{
	GENERATED_BODY()
    

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TabClasses")
    TArray<TSubclassOf<UPoolShopTabBase>> tabWidgetClasses;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab")
    TArray<UPoolShopTabBase*> tabs;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tab")
    UPoolShopTabBase* activeTab;
    
    UFUNCTION(BlueprintCallable)
    void ActivateTab(UPoolShopTabBase* tab);

    virtual void NativeConstruct();
    

    void UpdateItemDurability(EConsumableType consType,
                              int num,
                              int durability);
    UFUNCTION(BlueprintCallable)
    virtual void Refresh();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ShopUpdating")
    void ClickOnOwnedItem(UPoolShopItemWidgetBase* widget);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ShopUpdating")
    void ClickOnNotOwnedItem(UPoolShopItemWidgetBase* widget);
    UFUNCTION(BlueprintCallable)
    virtual FDynamicItemInfo GetDynamicItemInfo(EConsumableType consType, int num);
    
    
    
    
    UFUNCTION(BlueprintCallable)
    void ChangeActiveItem(EConsumableType consType,
                          int num);
    UFUNCTION(BlueprintCallable)
    void RefreshItem(EConsumableType consType,
                          int num);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "ShopUpdating")
    void AttachItems(UPoolShopTabBase* tab);
    
    void HideNotOwned(bool remove);

};
