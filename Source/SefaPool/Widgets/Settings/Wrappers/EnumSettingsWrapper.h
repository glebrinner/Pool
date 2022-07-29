// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Widgets/Settings/Wrappers/BaseSettingsWrapper.h"
#include "EnumSettingsWrapper.generated.h"



UCLASS()
class SEFAPOOL_API UEnumSettingsWrapper : public UBaseSettingsWrapper
{
	GENERATED_BODY()
protected:
	//UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	//class UComboBoxString* SettingsComboBox;

	void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void OnSelectionChanged(int selectedIdx);
    
    virtual void Update() override;
    
    UFUNCTION(BlueprintImplementableEvent)
    void UpdateEnum(int32 value);

	UFUNCTION(BlueprintImplementableEvent)
	void FillOptions(const TArray<FString>& values);

	virtual void Reset() override;
};
