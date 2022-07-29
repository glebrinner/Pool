// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolShopTabBase.h"
#include "PoolBaseShopWidget.h"
#include "Blueprint/WidgetTree.h"


void UPoolShopTabBase::Refresh(bool ignoreNotOwned)
{
    for (int i = 1; i < items.Num(); i++)
    {
        auto& item = items[i];
        //auto i = item_pair.Key;
        FDynamicItemInfo info = parent->GetDynamicItemInfo(consumableType, i);
        if (ignoreNotOwned && !info.is_owned)
            item->SetVisibility(ESlateVisibility::Collapsed);
        else
            item->SetVisibility(ESlateVisibility::Visible);
        if (item)
            item->Refresh();
        if (info.is_selected)
            active = item;
    }
}

void UPoolShopTabBase::NativeConstruct()
{
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    items.Empty();
    items.Add(nullptr);
    if (gi)
    {
        int max_tmp = 0;
        switch (consumableType)
        {
            case EConsumableType::CUE:
                max_tmp = PREDEFINED_CUES_N;
                break;
            case EConsumableType::CHALK:
                max_tmp = PREDEFINED_CHALKS_N;
                break;
            case EConsumableType::CLOTH:
                max_tmp = PREDEFINED_CLOTHS_N;
                break;
            case EConsumableType::TOKENPAK:
                max_tmp = PREDEFINED_TOKENPAKS_N;
                break;
            case EConsumableType::BUNDLE:
                max_tmp = PREDEFINED_BUNDLES_N;
                break;
        }
        
        const int max = max_tmp;
        
        for (int i = 1; i <= max; i++)
        {
            //FDynamicItemInfo info = parent->GetDynamicItemInfo(consumableType, i);
            
            if (!itemWidgetClass)
            {
                PRINTF((TEXT("UPoolShopTabBase::Init() item class is null")), Red)
            }
            auto item = CreateWidget<UPoolShopItemWidgetBase>(parent, itemWidgetClass);
            //item->AddToRoot();
            if (!item)
            {
                PRINTF((TEXT("UPoolShopTabBase::Init() item was not created")), Red)
            }
            item->parent = parent;
            item->id_ = i;
            item->consType = consumableType;
            items.Add(item);
            //items.Add(i, item);
            if (i == presetId)
                active = item;
        }
    }
    PRINTF((TEXT("UPoolShopTabBase::NativeConstruct %d %d"), static_cast<int>(consumableType), items.Num()), Yellow)
       Super::NativeConstruct();
}



void UPoolShopTabBase::ChangeActiveItemInTab(EConsumableType consType,
                      int num)
{
    if (consType != consumableType) return;
    
    PRINTF((TEXT("ChangeActiveItemInTab")), Yellow)
    
    if (active)
        active->Deactivate();
    else
        PRINTF((TEXT("NO active")), Yellow)
    active = items[num];
    if (active)
        active->Activate();
}

void UPoolShopTabBase::UpdateItemDurability(EConsumableType consType,
                          int num,
                          int durability)
{
    if (consType != consumableType) return;
    items[num]->Update(durability);
}
void UPoolShopTabBase::RefreshItem(EConsumableType consType,
                      int num)
{
    if (consType != consumableType) return;
    items[num]->Refresh();
}
