// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SefaPool/Widgets/Settings/MainSettingsWidget.h"
#include "BaseSettingsWrapper.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UBaseSettingsWrapper : public UUserWidget
{
	GENERATED_BODY()

	
protected:
    UPROPERTY()
	class UMainSettingsWidget* OwnerWidget;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SettingDescription;

	
public:
    const FWidgetPrepInfo* DataInstance;
	void SetData(const FWidgetPrepInfo& Data);

	//void SetDelegate(std::function<void(class UPoolGameSettings*,void*)>& Delegate) { DataInstance->Delegate = Delegate; }

	void SetOwnerWidget(UMainSettingsWidget* WidgetInstance) { OwnerWidget = WidgetInstance; }
    
    UFUNCTION(BlueprintCallable)
    virtual void Update();

	UFUNCTION(BlueprintCallable)
	virtual void ShowInfoPopup();

	virtual void Reset();
};
