// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolBaseWidget.h"
#include "LoadingWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API ULoadingWidget : public UPoolBaseWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "LoadingWidget")
	void ChangeVisibleText(const FString& text);
};
