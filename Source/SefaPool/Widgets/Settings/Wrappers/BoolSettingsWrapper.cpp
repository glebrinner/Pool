// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Widgets/Settings/Wrappers/BoolSettingsWrapper.h"

#include "Components/CheckBox.h"

void UBoolSettingsWrapper::NativeConstruct()
{
	Super::NativeConstruct();
	SettingCheckBox->OnCheckStateChanged.AddDynamic(this,&UBoolSettingsWrapper::OnCheckBoxStateChanged);
}

void UBoolSettingsWrapper::OnCheckBoxStateChanged(bool NewState)
{
	if (DataInstance->SetterFunction)
	{
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(DataInstance->SetterFunction, &NewState);
	}
}


void UBoolSettingsWrapper::Update()
{
    bool value = false;
    if (DataInstance->GetterFunction)
    {
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(DataInstance->GetterFunction, &value);
    }
    UpdateBool(value);
}

void UBoolSettingsWrapper::Reset()
{
    if (DataInstance->SetterFunction)
    {
        bool default_ = DataInstance->DefaultBool;
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(
            DataInstance->SetterFunction, 
            &default_
        );
        UpdateBool(default_);
    }
}