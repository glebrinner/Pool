// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolBaseShopWidget.h"
#include "Blueprint/WidgetTree.h"


FDynamicItemInfo UPoolBaseShopWidget::GetDynamicItemInfo(EConsumableType consType, int num)
{
    FDynamicItemInfo info;
    return info;
}

void UPoolBaseShopWidget::Refresh()
{
    
}

void UPoolBaseShopWidget::NativeConstruct()
{
    for (auto tab_type : tabWidgetClasses)
    {
        if (!tab_type)
        {
            UE_LOG(LogTemp, Error, TEXT("UPoolBaseShopWidget::NativeConstruct() tab class is null"));
        }
        auto tab = WidgetTree->ConstructWidget<UPoolShopTabBase>(tab_type);
        if (!tab)
        {
            UE_LOG(LogTemp, Error, TEXT("UPoolBaseShopWidget::NativeConstruct() tab was not created"));
        }
        tab->parent = this;
        tabs.Add(tab);
    }
    activeTab = nullptr;
    Super::NativeConstruct();
}

void UPoolBaseShopWidget::ChangeActiveItem(EConsumableType consType,
                      int num)
{
    for (auto & tab : tabs)
    {
        tab->ChangeActiveItemInTab(consType, num);
    }
}


void UPoolBaseShopWidget::ActivateTab(UPoolShopTabBase* tab)
{
    if (activeTab)
        activeTab->SetVisibility(ESlateVisibility::Collapsed);
    activeTab = tab;
    if (activeTab)
        activeTab->SetVisibility(ESlateVisibility::Visible);
}

void UPoolBaseShopWidget::UpdateItemDurability(EConsumableType consType,
                                               int num,
                                               int durability)
{
    UE_LOG(LogTemp, Error, TEXT("UPoolShopTabBase::UpdateItemDurability %x"), this);
    for (auto & tab : tabs)
    {
        tab->UpdateItemDurability(consType, num, durability);
    }
}
void UPoolBaseShopWidget::RefreshItem(EConsumableType consType,
                      int num)
{
    for (auto & tab : tabs)
    {
        tab->RefreshItem(consType, num);
    }
}


void UPoolBaseShopWidget::HideNotOwned(bool hide)
{
    for (auto & tab : tabs)
    {
        tab->Refresh(hide);
    }
}
