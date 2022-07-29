// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Widgets/Settings/Wrappers/EnumSettingsWrapper.h"

#include "Components/ComboBox.h"
#include "Components/ComboBoxString.h"

void UEnumSettingsWrapper::NativeConstruct()
{
    Super::NativeConstruct();

    FillOptions(DataInstance->BoxParams);
	/*for (int32 i = 0; i < DataInstance->BoxParams.Num(); ++i)
	{
		SettingsComboBox->AddOption(DataInstance->BoxParams[i]);
	}
	SettingsComboBox->OnSelectionChanged.AddDynamic(this,&UEnumSettingsWrapper::OnSelectionChanged);*/
}

void UEnumSettingsWrapper::OnSelectionChanged(int selectedIdx)
{
    UE_LOG(LogTemp, Warning, TEXT("SetterFunction: %x"), DataInstance->SetterFunction)
    if (DataInstance->SetterFunction)
    {
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(DataInstance->SetterFunction, &selectedIdx);
    }
}

void UEnumSettingsWrapper::Update()
{
    int32 value = 0;
    if (DataInstance->GetterFunction)
    {
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(DataInstance->GetterFunction, &value);
    }
    UpdateEnum(value);
}

void UEnumSettingsWrapper::Reset()
{
    if (DataInstance->SetterFunction)
    {
        int default_ = DataInstance->DefaultEnum;
        OwnerWidget->GetPoolGameSettings()->ProcessEvent(
            DataInstance->SetterFunction,
            &default_
        );
        UpdateEnum(default_);
    }
}