// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Widgets/Settings/Wrappers/BaseSettingsWrapper.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

#include "Components/TextBlock.h"

void UBaseSettingsWrapper::SetData(const FWidgetPrepInfo& Data)
{
	DataInstance = &Data;
	SettingDescription->SetText(FText::FromString(DataInstance->SettingsParamName));
// 	TArray<FWidgetPrepInfo> test(OwnerWidget->prepWidgets);
// 	test.IsValidIndex(0);
}

void UBaseSettingsWrapper::Update()
{
    
}


void UBaseSettingsWrapper::Reset()
{

}

void UBaseSettingsWrapper::ShowInfoPopup()
{
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        gi->ShowPopup(DataInstance->SettingsParamName, DataInstance->SettingDescription, TEXT("Ok"));
    }
}
