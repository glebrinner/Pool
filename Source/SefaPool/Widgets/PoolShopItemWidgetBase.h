// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "PoolShopItemWidgetBase.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FDynamicItemInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        int durability = -1;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        bool is_selected = false;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
        bool is_owned = false;
};



class UPoolBaseShopWidget;

UCLASS()
class SEFAPOOL_API UPoolShopItemWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = "true", InstanceEditable = "true"))
    EConsumableType consType;
    UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = "true", InstanceEditable = "true"))
    int id_;
    
    UPROPERTY(BlueprintReadWrite)
    UPoolBaseShopWidget* parent;
    
public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void Activate();
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void Deactivate();
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void Update(int durability);
    
    
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void Refresh();
    
    /*UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void LoadStatics(UShopStatics* shopStatics);
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void LoadDynamics(FDynamicItemInfo info);
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void Init();*/
    
};
