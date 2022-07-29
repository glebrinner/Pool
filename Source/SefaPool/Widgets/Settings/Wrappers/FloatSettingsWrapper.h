// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Widgets/Settings/Wrappers/BaseSettingsWrapper.h"
#include "FloatSettingsWrapper.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UFloatSettingsWrapper : public UBaseSettingsWrapper
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USlider* SettingSlider;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SliderValueText;
    
	float MinConstraint;

	float MaxConstraint;

	void NativeConstruct() override;

	UFUNCTION()
	void OnSliderValueChanged(float NewValue);

    virtual void Update() override;
    
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateFloat(float value, float step);

	virtual void Reset() override;
};
