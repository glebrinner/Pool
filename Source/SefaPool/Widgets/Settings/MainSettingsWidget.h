// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <functional>
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SefaPool/Widgets/PoolBaseWidget.h"
#include "SefaPool/Data/SefaPoolSettings.h"
#include "MainSettingsWidget.generated.h"

UENUM()
enum E_WIDGET_TYPE
{
	FLOAT,
	INTEGER,
	BOOL_,
	STRING,
	VECTOR2D,
	COMBO_BOX,
	SUBCATEGORY
};
USTRUCT()
struct FWidgetPrepInfo
{
	GENERATED_BODY()
	FWidgetPrepInfo()
	{
		SetterFunction = nullptr;
		SettingsParamName = "";
		SettingDescription = "";
		type = E_WIDGET_TYPE::FLOAT;
		MinValue = 0.f;
		MaxValue = 0.f;
		BoxParams = TArray<FString>();
	}
	/*FWidgetPrepInfo(const FWidgetPrepInfo & other)
	{
		UE_LOG(LogTemp,Error,TEXT("Used copy constructor for FWidgetPrepInfo struct"))
	}*/
	
	// FWidgetPrepInfo operator=(const FWidgetPrepInfo& other) = delete;
	/*FWidgetPrepInfo(UFunction* setter,
                    FString ParamName,
                    E_WIDGET_TYPE WidType,
                    float min = 0.f,
                    float max = 1.f,
                    TArray<FString> Params =  TArray<FString>() )
	{
        SetterFunction = setter;
		SettingsParamName = ParamName;
		type = WidType;
		MinValue = min;
		MaxValue = max;
		BoxParams = Params;
	}*/
    UFunction* SetterFunction;
    UFunction* GetterFunction;
	FString SettingsParamName;
	FString SettingDescription;
	E_WIDGET_TYPE type;
	float MinValue = 0.f;
	float MaxValue = 1.f;
    float Step = 0.1f;
	union {
		float DefaultFloat;
		bool DefaultBool;
		int DefaultEnum;
	};
	
	TArray<FString> BoxParams;
    FString category;
};

class USettingsTabWidget;

UCLASS()
class SEFAPOOL_API UMainSettingsWidget : public UPoolBaseWidget
{
	GENERATED_BODY()
    UPROPERTY()
	class USefaPoolSettings* PoolGameSettings;
	
public:

	// todo Для других виджетов аналогично
	// UPROPERTY(EditDefaultsOnly)
	// TSubclassOf<class UGameSettingsWidget> GameSettingsWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UFloatSettingsWrapper> FloatSettingsWrapperClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UEnumSettingsWrapper>  EnumSettingsWrapperClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UBoolSettingsWrapper>  BoolSettingsWrapperClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USubcategorySettingsWrapper>  SubcategorySettingsWrapperClass;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<USettingsTabWidget>          SettingsTabWidgetClass;
                                             
protected:
    UPROPERTY(BlueprintReadOnly)
    TArray<USettingsTabWidget*>              settingsTabWidgets;
    
    UPROPERTY(BlueprintReadOnly)
    TArray<FString>                          settingsTabNames;

	void NativeConstruct() override;

	void GenerateSettingsWidget();
    
    UFUNCTION(BlueprintCallable)
    void FullUpdate();
    
    UFUNCTION(BlueprintImplementableEvent)
    void AddCategoryTabButton(
		const FString& category, 
		USettingsTabWidget* tab, 
		int start, 
		int len, 
		int total);
    
    UFUNCTION(BlueprintCallable)
    void ChangeCategoryTab(USettingsTabWidget* tab);

	UPROPERTY(BlueprintReadOnly)
	USettingsTabWidget* currentTab;
	
public:

	TArray<FWidgetPrepInfo> prepWidgets;
	
    USefaPoolSettings* GetPoolGameSettings() const { return PoolGameSettings; }
};
