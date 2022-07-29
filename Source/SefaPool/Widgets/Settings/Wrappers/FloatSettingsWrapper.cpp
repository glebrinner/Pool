// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Widgets/Settings/Wrappers/FloatSettingsWrapper.h"

#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "SefaPool/Widgets/Settings/MainSettingsWidget.h"

void UFloatSettingsWrapper::NativeConstruct()
{
	Super::NativeConstruct();
	SettingSlider->OnValueChanged.AddDynamic(this,&UFloatSettingsWrapper::OnSliderValueChanged);
	SettingSlider->SetMinValue(DataInstance->MinValue);
	SettingSlider->SetMaxValue(DataInstance->MaxValue);
    SettingSlider->SetStepSize(DataInstance->Step);
}

void UFloatSettingsWrapper::OnSliderValueChanged(float NewValue)
{
	//SliderValueText->SetText(FText::FromString(FString::SanitizeFloat(NewValue,5)));
    UpdateFloat(NewValue, DataInstance->Step);
    
    if (DataInstance->SetterFunction)
    {
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(DataInstance->SetterFunction, &NewValue);
    }
	/*if (DataInstance->Delegate)
	{
		DataInstance->Delegate(OwnerWidget->GetPoolGameSettings(),&(NewValue));
	}*/
}


void UFloatSettingsWrapper::Update()
{
    float value = 0;
    if (DataInstance->GetterFunction)
    {
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(DataInstance->GetterFunction, &value);
        UpdateFloat(value, DataInstance->Step);
    }
}

void UFloatSettingsWrapper::Reset()
{
    if (DataInstance->SetterFunction)
    {
        UE_LOG(LogTemp, Error, TEXT("UFloatSettingsWrapper::Reset resetting"))
        float default_ = DataInstance->DefaultFloat;
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(
            DataInstance->SetterFunction,
            &default_
        );
        UpdateFloat(default_, DataInstance->Step);
    }
}