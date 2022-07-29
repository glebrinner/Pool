// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Widgets/PoolBaseWidget.h"
#include "InteractivePopupWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE(FPopupCallback);

UCLASS()
class SEFAPOOL_API UInteractivePopupWidget : public UPoolBaseWidget
{
	GENERATED_BODY()
public:

    UPROPERTY()
    FPopupCallback left;
    UPROPERTY()
    FPopupCallback right;

    UFUNCTION(BlueprintCallable)
    virtual void RegisterCallbacks(const FPopupCallback& left_,
                                   const FPopupCallback& right_);
    UFUNCTION(BlueprintCallable)
    void CallLeft();
    UFUNCTION(BlueprintCallable)
    void CallRight();
	
};
