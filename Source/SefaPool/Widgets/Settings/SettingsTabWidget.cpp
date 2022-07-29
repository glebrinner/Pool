// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsTabWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/ScrollBox.h"
#include "Components/Spacer.h"
#include "Wrappers/FloatSettingsWrapper.h"
#include "Wrappers/EnumSettingsWrapper.h"
#include "Wrappers/BoolSettingsWrapper.h"
#include "Wrappers/SubcategorySettingsWrapper.h"



void USettingsTabWidget::Refresh()
{
    for (auto& itemWidget : itemWidgets)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("USettingsTabWidget::Refresh() wid %x, inst %x, Updating %d"),
               itemWidget, itemWidget->DataInstance,
               itemWidget->DataInstance->SettingsParamName.Len())
        if (itemWidget)
            itemWidget->Update();
    }
}

void USettingsTabWidget::GenerateWidgetForItem(const FWidgetPrepInfo& wpi)
{
    if (!parent) return;
    UE_LOG(LogTemp,Error,TEXT("USettingsTabWidget::GenerateWidgetForItem starting"))
    TSubclassOf<class UBaseSettingsWrapper> widgetClass = nullptr;
    
    switch (wpi.type)
    {
        case FLOAT:
            widgetClass = parent->FloatSettingsWrapperClass;
            break;
        case COMBO_BOX:
            widgetClass = parent->EnumSettingsWrapperClass;
            break;
        case BOOL_:
            widgetClass = parent->BoolSettingsWrapperClass;
            break;
        case SUBCATEGORY:
            UE_LOG(LogTemp, Error, TEXT("USettingsTabWidget::GenerateWidgetForItem Found subcat"))
            widgetClass = parent->SubcategorySettingsWrapperClass;
            break;
        default:
            UE_LOG(LogTemp,Error,TEXT("USettingsTabWidget::GenerateWidgetForItem default switch case %d"), wpi.type)
            return;
            break;
    }
    UBaseSettingsWrapper* wrapWI = WidgetTree->ConstructWidget<UBaseSettingsWrapper>(widgetClass);
    wrapWI->SetData(wpi);
    wrapWI->SetOwnerWidget(parent);
    MainScrollBox->AddChild(wrapWI);
    USpacer* spacer = WidgetTree->ConstructWidget<USpacer>(USpacer::StaticClass());
    spacer->SetSize(FVector2D(1.0f, 30.0f));
    MainScrollBox->AddChild(spacer);
    UE_LOG(LogTemp,Error,TEXT("USettingsTabWidget::GenerateWidgetForItem all good"))
    itemWidgets.Add(wrapWI);
}


void USettingsTabWidget::NativeConstruct()
{
    for (auto& itemInfo : itemInfos)
        GenerateWidgetForItem(itemInfo);
    Super::NativeConstruct();
}

void USettingsTabWidget::RegisterItem(FWidgetPrepInfo&& wpi)
{
    itemInfos.Add(wpi);
}

void USettingsTabWidget::Reset()
{
    for (auto& itemWidget : itemWidgets)
    {
        if (itemWidget)
        {
            UE_LOG(LogTemp, Error, TEXT("USettingsTabWidget::Reset iteration"))
            itemWidget->Reset();
        }
    }
}