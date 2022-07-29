// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolBaseShopWidget.h"
#include "PoolInGameShopWidget.generated.h"

/**
 * 
 */

UCLASS()
class SEFAPOOL_API UPoolInGameShopWidget : public UPoolBaseShopWidget
{
    GENERATED_BODY()
public:
    
    virtual void NativeConstruct();
    virtual FDynamicItemInfo GetDynamicItemInfo(EConsumableType consType, int num);
    
    virtual void Refresh();
};

