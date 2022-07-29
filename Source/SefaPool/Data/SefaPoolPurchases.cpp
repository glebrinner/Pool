// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPoolPurchases.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPoolMessages.h"
void USefaPoolPurchases::Init(USefaPoolGameInstance* gi_)
{
    Super::Init(gi_);
    UE_LOG(LogTemp, Warning, TEXT("purchase initialize"))
}


void USefaPoolPurchases::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "purchases", 0);
}

void USefaPoolPurchases::Load()
{
    USefaPoolPurchases* LoadedGame = Cast<USefaPoolPurchases>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "purchases", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load items")), Yellow)
    }
    else
    {
        itemsPurchaseArray = LoadedGame->itemsPurchaseArray;
        PRINTF((TEXT("Items successfully loaded")), Green)
    }
}

void from_json(const cool_json_t& json, FPurchasedItem& pItem)
{
    if (!json.contains("type") || !json.contains("id") ||
       !json.contains("timestamp"))
        return;

    if (json["type"] == "cloth")
        pItem.itemType = EConsumableType::CLOTH;
    else if (json["type"] == "cue")
        pItem.itemType = EConsumableType::CUE; 
    else if (json["type"] == "chalk")
        pItem.itemType = EConsumableType::CHALK;
    else if (json["type"] == "tokenpak")
        pItem.itemType = EConsumableType::TOKENPAK;
    else if (json["type"] == "bundle")
        pItem.itemType = EConsumableType::BUNDLE;
    json["type"].get_to(pItem.typeName);
    json["id"].get_to(pItem.item_id);
    int64 timestamp = 0;
    json["timestamp"].get_to(timestamp);
    pItem.timestamp = FDateTime::FromUnixTimestamp(timestamp);

}

void USefaPoolPurchases::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)> && successful,
    const std::function<void(USefaPoolDataBlock*)> && unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }

    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "shop/get_purchase_history"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    Purchase: %s"), *(FString(resp_json.dump(2).c_str())))
            if (resp_json.contains("items") &&
                resp_json["items"].is_array() &&
                resp_json["items"].size() > 0)
            {
                itemsPurchaseArray.Empty();
                for (auto purchase_ : resp_json["items"])
                {
                    if (!purchase_.contains("id") ||
                        !purchase_.contains("type") || 
                        (purchase_["type"] != "cue" &&
                         purchase_["type"] != "chalk" &&
                         purchase_["type"] != "cloth" &&
                         purchase_["type"] != "bundle" &&
                         purchase_["type"] != "tokenpak"))
                        continue;
                    FPurchasedItem addPurchase;
                    purchase_.get_to(addPurchase);
                    itemsPurchaseArray.Add(addPurchase);
                }
                if (successful) successful(this);
                Save();
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                //UE_LOG(LogTemp, Warning, TEXT("user/get_all_messages did not messages"))
            }
        }
        else if (unsuccessful) unsuccessful(this);
    };

    gi->MakeRequest("shop", "get_purchase_history", cool_json_t(), callback);
}


TArray<FPurchasedItem> USefaPoolPurchases::GetPurchasedItemsFiltered(FString filter)
{
    if (filter.Len() == 0) return itemsPurchaseArray;
    TArray<FPurchasedItem> ret;

    for (auto& purchase_ : itemsPurchaseArray)
    {
        if (filter.IsEmpty() || purchase_.typeName.ToLower().Contains(filter.ToLower()))
            ret.Add(purchase_);
    }
    return ret;
}