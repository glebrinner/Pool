// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolBaseShopWidget.h"
#include "PoolMainShopWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolMainShopWidget : public UPoolBaseShopWidget
{
	GENERATED_BODY()
public:
    //UPoolMainShopWidget();
    
    UFUNCTION(BlueprintCallable, Category = "Requests")
    void RequestBuyItem(EConsumableType type,
                        int id_,
                        const FResponseDelegat& succesful,
                        const FResponseDelegat& unsuccesful);
    
    UFUNCTION(BlueprintCallable, Category = "Requests")
    void RequestChangeItem(EConsumableType type,
                           int id_,
                           const FResponseDelegat& succesful,
                           const FResponseDelegat& unsuccesful);
    
    virtual void NativeConstruct();
    virtual FDynamicItemInfo GetDynamicItemInfo(EConsumableType consType, int num);
    
    virtual void Refresh();
};
