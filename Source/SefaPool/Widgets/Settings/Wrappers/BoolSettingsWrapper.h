// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Widgets/Settings/Wrappers/BaseSettingsWrapper.h"
#include "BoolSettingsWrapper.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UBoolSettingsWrapper : public UBaseSettingsWrapper
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UCheckBox* SettingCheckBox;

	void NativeConstruct() override;

	UFUNCTION()
	void OnCheckBoxStateChanged(bool NewState);
    
    virtual void Update() override;
    
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateBool(bool state);

	virtual void Reset() override;

};
