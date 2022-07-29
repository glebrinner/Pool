// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolInGameShopWidget.h"
#include "../Shared/PoolPlayerControllerBase.h"


void UPoolInGameShopWidget::NativeConstruct()
{
    Super::NativeConstruct();
    for (auto & tab : tabs)
    {
        if (!tab) continue;
        if (gi)
        {
            switch (tab->consumableType)
            {
                case EConsumableType::CUE:
                    tab->presetId = gi->data->inventory->current_cue;
                    break;
                case EConsumableType::CHALK:
                    tab->presetId = gi->data->inventory->current_chalk;
                    break;
                case EConsumableType::CLOTH:
                    tab->presetId = gi->data->inventory->current_cloth;
                    break;
            }
        }
    }
}

FDynamicItemInfo UPoolInGameShopWidget::GetDynamicItemInfo(EConsumableType consType, int num)
{
    FDynamicItemInfo info = {};
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return info;
    
    APoolPlayerStateBase* ps = dynamic_cast<APoolPlayerStateBase*>(pc->PlayerState);
    
    if (ps)
    {
        switch (consType)
        {
            case EConsumableType::CUE:
            {
                info.durability = ps->GetItemDurability().cue_durabilities[num];
                info.is_selected = ps->cueInfo.cueNum == num;
                info.is_owned = ps->CanSelectCue(num);
                break;
            }
            case EConsumableType::CHALK:
            {
                info.durability = ps->GetItemDurability().chalk_durabilities[num];
                info.is_selected = ps->cueInfo.chalkNum == num;
                info.is_owned = ps->CanSelectChalk(num);
                break;
            }
            case EConsumableType::CLOTH:
            {
                info.durability = 0;
                info.is_selected = 1 == num;
                info.is_owned = true; //num != 4;
                break;
            }
        }
    }
    return info;
}


void UPoolInGameShopWidget::Refresh()
{
    for (auto & tab : tabs)
    {
        tab->Refresh(true);
    }
}
