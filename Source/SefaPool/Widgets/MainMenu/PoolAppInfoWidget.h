// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Widgets/PoolBaseWidget.h"
#include "PoolAppInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolAppInfoWidget : public UPoolBaseWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
    FString version;
    UPROPERTY(BlueprintReadOnly)
    FString build;
    
    virtual void NativeConstruct();
	
};
