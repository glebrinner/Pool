
#include "SefaPoolInventory.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolInventory::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "inventory", 0);
}
    
void USefaPoolInventory::Load()
{
    USefaPoolInventory* LoadedGame = Cast<USefaPoolInventory>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "inventory", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load inventory")), Yellow)
    }
    else
    {
        cue_durabilities = LoadedGame->cue_durabilities;
        current_cue = LoadedGame->current_cue;
        chalk_durabilities = LoadedGame->chalk_durabilities;
        current_chalk = LoadedGame->current_chalk;
        cloth_ownership = LoadedGame->cloth_ownership;
        current_cloth = LoadedGame->current_cloth;
        bundle_ownership = LoadedGame->bundle_ownership;
        balance = LoadedGame->balance;
        PRINTF((TEXT("Inventory successfully loaded")), Green)
    }
}
    
void USefaPoolInventory::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                           const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback_get_bought_items = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "shop/get_bought_items"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    SHOP: %s"), *(FString(resp_json.dump(2).c_str())))
            if (resp_json.contains("cues") &&
                resp_json.contains("chalks") &&
                resp_json.contains("cloths") &&
                resp_json.contains("bundles"))
            {
                cue_durabilities.Init(-1, PREDEFINED_CUES_N + 1);
                for (const auto& cue_pair : resp_json["cues"].items())
                {
                    cue_durabilities[std::stoi(cue_pair.key())] = cue_pair.value().get<int>();
                }
                
                chalk_durabilities.Init(-1, PREDEFINED_CUES_N + 1);
                for (const auto& chalk_pair : resp_json["chalks"].items())
                {
                    chalk_durabilities[std::stoi(chalk_pair.key())] = chalk_pair.value().get<int>();
                }
                
                cloth_ownership.Init(false, PREDEFINED_CLOTHS_N + 1);
                int cloth_n = 1;
                for (const auto& cloth : resp_json["cloths"])
                {
                    cloth_ownership[cloth.get<int> ()] = true;
                    cloth_n++;
                }
                
                bundle_ownership.Init(false, PREDEFINED_BUNDLES_N + 1);
                int bundle_n = 1;
                for (const auto& bundle : resp_json["bundles"])
                {
                    bundle_ownership[bundle.get<int> ()] = true;
                    bundle_n++;
                }
                
                if (successful) successful(this);
                Save();
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                UE_LOG(LogTemp, Warning, TEXT("lobbyrelay/get_lobby did not contain cues, chalks, or cloths"))
            }
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("shop", "get_bought_items", cool_json_t(), callback_get_bought_items);
    
    auto callback_get_player_info = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/get_player_info"))
        {
            if (resp_json.contains("active_cue") &&
                resp_json.contains("active_chalk") &&
                resp_json.contains("active_cloth") &&
                resp_json.contains("balance"))
            {
                resp_json["active_cue"].get_to(current_cue);
                resp_json["active_chalk"].get_to(current_chalk);
                resp_json["active_cloth"].get_to(current_cloth);
                resp_json["balance"].get_to(balance);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("user/get_player_info did not contain active_cue, active_chalk, active_cloth or balance"))
            }
        }
    };
    
    gi->MakeRequest("user", "get_player_info", cool_json_t(), callback_get_player_info);
}
