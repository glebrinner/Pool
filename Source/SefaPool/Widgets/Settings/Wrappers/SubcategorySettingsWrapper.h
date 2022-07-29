// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Widgets/Settings/Wrappers/BaseSettingsWrapper.h"
#include "SubcategorySettingsWrapper.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API USubcategorySettingsWrapper : public UBaseSettingsWrapper
{
	GENERATED_BODY()
protected:
	void NativeConstruct() override;
    
    virtual void Update() override;
};
