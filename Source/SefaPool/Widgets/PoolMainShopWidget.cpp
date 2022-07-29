// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolMainShopWidget.h"
#include "SefaPool/Data/SefaPoolShop.h"

void UPoolMainShopWidget::RequestBuyItem(EConsumableType type,
                                         int id_,
                                         const FResponseDelegat& succesful,
                                         const FResponseDelegat& unsuccesful)
{
    
    if (gi)
    {
        gi->data->shop->ProcessBuyItem(type, id_, succesful, unsuccesful);
    }
}

void UPoolMainShopWidget::RequestChangeItem(EConsumableType type,
                                            int id_,
                                            const FResponseDelegat& succesful,
                                            const FResponseDelegat& unsuccesful)
{
    
    if (gi)
    {
        FString endpoint = TEXT("change_active_item");
        std::string item_type;
        bool active = false;
        switch(type)
        {
            case EConsumableType::CUE:
                item_type = "cue";
                active = true;
                break;
            case EConsumableType::CHALK:
                item_type = "chalk";
                active = true;
                break;
            case EConsumableType::CLOTH:
                item_type = "cloth";
                active = true;
                break;
                
        }
        
        if (!active)
        {
            PRINTF((TEXT("In development")), Red)
            return;
        }
        
        cool_json_t json;
        json["item_id"] = id_;
        json["type"] = item_type;
        
        auto callback = [succesful, unsuccesful, id_, type, this](cool_json_t resp_json) -> void {
            if (check_status(resp_json, "user/change_active_item"))
            {
                if (resp_json.contains("changed") &&
                    resp_json["changed"].get<bool> ())
                {
                    switch(type)
                    {
                        case EConsumableType::CUE:
                            gi->data->inventory->current_cue = id_;
                            break;
                        case EConsumableType::CHALK:
                            gi->data->inventory->current_chalk = id_;
                            break;
                        case EConsumableType::CLOTH:
                            gi->data->inventory->current_cloth = id_;
                            break;
                    }
                    ChangeActiveItem(type, id_);
                    succesful.Execute();
                }
                else
                {
                    unsuccesful.Execute();
                }
            }
        };
        
        gi->MakeRequest("user", endpoint, json, callback);
    }
}

void UPoolMainShopWidget::NativeConstruct()
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

FDynamicItemInfo UPoolMainShopWidget::GetDynamicItemInfo(EConsumableType consType, int num)
{
    FDynamicItemInfo info = {};
    
    if (gi)
    {
        switch (consType)
        {
            case EConsumableType::CUE:
            {
                info.durability = gi->data->inventory->cue_durabilities[num];
                info.is_selected = gi->data->inventory->current_cue == num;
                info.is_owned = gi->data->inventory->cue_durabilities[num] != -1;
                break;
            }
            case EConsumableType::CHALK:
            {
                info.durability = gi->data->inventory->chalk_durabilities[num];
                info.is_selected = gi->data->inventory->current_chalk == num;
                info.is_owned = gi->data->inventory->chalk_durabilities[num] != -1;
                break;
            }
            case EConsumableType::CLOTH:
            {
                info.durability = 0;
                info.is_selected = gi->data->inventory->current_cloth == num;
                info.is_owned =  gi->data->inventory->cloth_ownership[num];
                
                    printf("cloth owned [%d] = %d\n", num, info.is_owned);//num != 4;
                break;
            }
            case EConsumableType::TOKENPAK:
            {
                info.durability = 0;
                info.is_selected = false;
                info.is_owned =  false;
                break;
            }
            case EConsumableType::BUNDLE:
            {
                info.durability = 0;
                info.is_selected = false;
                info.is_owned = gi->data->inventory->bundle_ownership[num];
                PRINTF((TEXT("BUNDLE %d is %s owned"), num, info.is_owned ? TEXT("") : TEXT("NOT")), Yellow)
                break;
            }
        }
    }
    return info;
}


void UPoolMainShopWidget::Refresh()
{
    for (auto & tab : tabs)
    {
        tab->Refresh(false);
    }
}
