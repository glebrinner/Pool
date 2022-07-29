// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Widgets/Settings/MainSettingsWidget.h"

#include "Components/ScrollBox.h"
#include "SefaPool/Widgets/Settings/Wrappers/FloatSettingsWrapper.h"
#include "SefaPool/Widgets/Settings/Wrappers/EnumSettingsWrapper.h"
#include "SefaPool/Widgets/Settings/Wrappers/BoolSettingsWrapper.h"
#include "SettingsTabWidget.h"
#include "Components/Spacer.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "Blueprint/WidgetTree.h"

void UMainSettingsWidget::NativeConstruct()
{
    if (!gi)
    {
        UE_LOG(LogTemp, Error, TEXT("Null GI in %s"), *FString(__PRETTY_FUNCTION__))
        return;
    }
    PoolGameSettings = gi->data->settings;
    settingsTabNames.Empty();
    
    #define ADD_WIDGET_FOR_FLOAT_SETTING(sett) \
    prepWidgets.Add(FWidgetPrepInfo()); \
    prepWidgets.Last().type = FLOAT; \
    prepWidgets.Last().SetterFunction = PoolGameSettings->FindFunction(TEXT("Set"#sett)); \
    prepWidgets.Last().GetterFunction = PoolGameSettings->FindFunction(TEXT("Get"#sett)); \
    prepWidgets.Last().MaxValue = PoolGameSettings->sett##Info.max; \
    prepWidgets.Last().MinValue = PoolGameSettings->sett##Info.min; \
    prepWidgets.Last().Step = PoolGameSettings->sett##Info.step; \
    prepWidgets.Last().DefaultFloat = PoolGameSettings->sett##Info.default_; \
    prepWidgets.Last().SettingsParamName = PoolGameSettings->sett##Info.name; \
    prepWidgets.Last().category = PoolGameSettings->sett##Info.category; \
    prepWidgets.Last().SettingDescription = PoolGameSettings->sett##Info.description; \
    settingsTabNames.AddUnique(PoolGameSettings->sett##Info.category);

#define ADD_WIDGET_FOR_SUBCATEGORY(name, cat) \
    prepWidgets.Add(FWidgetPrepInfo()); \
    prepWidgets.Last().type = SUBCATEGORY; \
    prepWidgets.Last().SettingsParamName = TEXT(name); \
    prepWidgets.Last().category = TEXT(cat); \
    settingsTabNames.AddUnique(TEXT(cat));
    
    #define ADD_WIDGET_FOR_BOOL__SETTING(sett) \
    prepWidgets.Add(FWidgetPrepInfo()); \
    prepWidgets.Last().type = BOOL_; \
    prepWidgets.Last().SetterFunction = PoolGameSettings->FindFunction(TEXT("Set"#sett)); \
    prepWidgets.Last().GetterFunction = PoolGameSettings->FindFunction(TEXT("Get"#sett)); \
    prepWidgets.Last().SettingsParamName = PoolGameSettings->sett##Info.name; \
    prepWidgets.Last().category = PoolGameSettings->sett##Info.category; \
    prepWidgets.Last().SettingDescription = PoolGameSettings->sett##Info.description; \
    prepWidgets.Last().DefaultBool = PoolGameSettings->sett##Info.default_; \
    settingsTabNames.AddUnique(PoolGameSettings->sett##Info.category);\
    if (!prepWidgets.Last().SetterFunction) UE_LOG(LogTemp, Error, TEXT("Getter for %s not found"), TEXT(#sett))
    
    
    //ADD_WIDGET_FOR_SETTING(name,param_name, param_type, wrapper_type,min_value,max_value);
	#define ADD_WIDGET_FOR_ENUM__SETTING(sett) \
    prepWidgets.Add(FWidgetPrepInfo()); \
    prepWidgets.Last().type = COMBO_BOX; \
    prepWidgets.Last().SetterFunction = PoolGameSettings->FindFunction(TEXT("Set"#sett"_int")); \
    prepWidgets.Last().GetterFunction = PoolGameSettings->FindFunction(TEXT("Get"#sett"_int")); \
    prepWidgets.Last().SettingsParamName = PoolGameSettings->sett##Info.name; \
	prepWidgets.Last().BoxParams = PoolGameSettings->sett##Info.values; \
    prepWidgets.Last().DefaultEnum = PoolGameSettings->sett##Info.default_; \
    prepWidgets.Last().category = PoolGameSettings->sett##Info.category; \
    prepWidgets.Last().SettingDescription = PoolGameSettings->sett##Info.description; \
    settingsTabNames.AddUnique(PoolGameSettings->sett##Info.category);
    /*
    ADD_WIDGET_FOR_SUBCATEGORY("Some input category", "Input");
    ADD_WIDGET_FOR_SUBCATEGORY("Some game category", "Game");
    ADD_WIDGET_FOR_SUBCATEGORY("Some physics category",  "Physics");
    ADD_WIDGET_FOR_SUBCATEGORY("Some graphics category", "Graphics");
    */
    ADD_WIDGET_FOR_SUBCATEGORY("Camera sensitivity", "Input");
    ADD_WIDGET_FOR_FLOAT_SETTING(FocusCameraHorizontalSensitivity);
	ADD_WIDGET_FOR_FLOAT_SETTING(FocusCameraVerticalSensitivity);
	ADD_WIDGET_FOR_FLOAT_SETTING(ViewCameraHorizontalSensitivity); 
    ADD_WIDGET_FOR_SUBCATEGORY("HUD sensitivity", "Input");
	ADD_WIDGET_FOR_FLOAT_SETTING(FocusPinchSensitivity);
	ADD_WIDGET_FOR_FLOAT_SETTING(FocusJoystickSensitivity);
	ADD_WIDGET_FOR_FLOAT_SETTING(Joystick3DSensitivity);
	ADD_WIDGET_FOR_FLOAT_SETTING(BallInHandSensitivity);
    ADD_WIDGET_FOR_SUBCATEGORY("HUD behavior", "Input");
    ADD_WIDGET_FOR_ENUM__SETTING(ShotStrength);
    ADD_WIDGET_FOR_BOOL__SETTING(TapToAim2D);
    ADD_WIDGET_FOR_FLOAT_SETTING(TapDelay);
    ADD_WIDGET_FOR_FLOAT_SETTING(DoublePinchTapSpeed);

    ADD_WIDGET_FOR_SUBCATEGORY("General", "Game");
	ADD_WIDGET_FOR_BOOL__SETTING(AutoTranslationBallInHand);
	ADD_WIDGET_FOR_BOOL__SETTING(HardMode);
    ADD_WIDGET_FOR_SUBCATEGORY("Camera modes", "Game");
    ADD_WIDGET_FOR_ENUM__SETTING(CameraMoveMode);
    ADD_WIDGET_FOR_ENUM__SETTING(PreferredCameraMode);
    ADD_WIDGET_FOR_SUBCATEGORY("Spectating camera", "Game");
    ADD_WIDGET_FOR_FLOAT_SETTING(SpectatingCamPosition);
    ADD_WIDGET_FOR_FLOAT_SETTING(SpectatingCamHeight);
    ADD_WIDGET_FOR_FLOAT_SETTING(SpectatingCamFOV);
    ADD_WIDGET_FOR_FLOAT_SETTING(SpectatingCamCenteringHeight);


    ADD_WIDGET_FOR_SUBCATEGORY("Engine performance", "Physics");
    ADD_WIDGET_FOR_FLOAT_SETTING(PhysSubstepSize);
    ADD_WIDGET_FOR_FLOAT_SETTING(SlidingResistanceVelocityDeltaTrigger);
    ADD_WIDGET_FOR_SUBCATEGORY("Material constants", "Physics");
	ADD_WIDGET_FOR_FLOAT_SETTING(RollingFriction);
	ADD_WIDGET_FOR_FLOAT_SETTING(SlipFrictionBallTable);
	ADD_WIDGET_FOR_FLOAT_SETTING(RestrictionBallCloth);
	ADD_WIDGET_FOR_FLOAT_SETTING(NormalVelocityLossValue);
    ADD_WIDGET_FOR_FLOAT_SETTING(VerticalAngleAngularDemping);
    ADD_WIDGET_FOR_FLOAT_SETTING(CoefBallMassDistribution);
    ADD_WIDGET_FOR_SUBCATEGORY("World constants", "Physics");
	ADD_WIDGET_FOR_FLOAT_SETTING(AccelerationOfGravity);
    ADD_WIDGET_FOR_SUBCATEGORY("Cushion physics", "Physics");
	ADD_WIDGET_FOR_FLOAT_SETTING(CoefBallSurfaceVelocityLoss);
	ADD_WIDGET_FOR_FLOAT_SETTING(CoefLossVelocityIntoLossLinearVelocity);
	ADD_WIDGET_FOR_FLOAT_SETTING(CoefSidewallProfile);

	ADD_WIDGET_FOR_BOOL__SETTING(DrawBallsShadows);
    UE_LOG(LogTemp,Error,TEXT("prep widget settings instance created"))
	
	
#undef ADD_WIDGET_FOR_FLOAT_SETTING
#undef ADD_WIDGET_FOR_BOOL__SETTING
#undef ADD_WIDGET_FOR_ENUM__SETTING

	

	// todo УСТАНОВИТЬ GameSettingsWidgetClass or crash bandicoot
	// GameSettingsWidget = CreateWidget<UGameSettingsWidget>(this,GameSettingsWidgetClass);
	// // todo Для других виджетов сделать аналогично
	// MainScrollBox->AddChild(GameSettingsWidget);
	// GameSettingsWidget->SetOwnerWidget(this);
	GenerateSettingsWidget();
	UE_LOG(LogTemp,Error,TEXT("Prep widget size is %i"),prepWidgets.Num())
    Super::NativeConstruct();
}

void UMainSettingsWidget::GenerateSettingsWidget()
{
    settingsTabWidgets.Empty();
    int totalLen = 0;
    for (auto& category : settingsTabNames)
    {
        totalLen += category.Len();
        totalLen += 6;
    }

    int idx = 0;
	for (auto& category: settingsTabNames)
	{
        UE_LOG(LogTemp, Warning, TEXT("Generating tab %s"), *category)
		if (category.Len() > 0)
        {
            auto tab = WidgetTree->ConstructWidget<USettingsTabWidget>(SettingsTabWidgetClass);
            tab->name = category;
            tab->parent = this;
            for (auto& pw: prepWidgets)
            {
                if (pw.category != category) continue;
                tab->RegisterItem(std::move(pw));
            }
            settingsTabWidgets.Add(tab);
            AddCategoryTabButton(category, tab, idx, category.Len() + 6, totalLen);
            idx += category.Len() + 6;
        }
	}
    ChangeCategoryTab(settingsTabWidgets[0]);
}

void UMainSettingsWidget::FullUpdate()
{
    for (auto& widget : settingsTabWidgets)
    {
        if (widget) widget->Refresh();
    }
}

void UMainSettingsWidget::ChangeCategoryTab(USettingsTabWidget* tab)
{
    if (settingsTabWidgets.Find(tab) != INDEX_NONE)
    {
        for (auto& settingsTab: settingsTabWidgets)
        {
            if (settingsTab)
            {
                if (settingsTab != tab)
                    settingsTab->SetVisibility(ESlateVisibility::Collapsed);
                else
                    settingsTab->SetVisibility(ESlateVisibility::Visible);
            }
        }
        
    }
    currentTab = tab;
}
