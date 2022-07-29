
#include "SefaPoolChampionship.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolChampionship::Save()
{
    //UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "championship", 0);
}
    
void USefaPoolChampionship::Load()
{
    /*USefaPoolChampionship* LoadedGame = Cast<USefaPoolChampionship>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "inventory", 0));
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
    }*/
}

void from_json(const cool_json_t& json, FChampionshipEntry& entry)
{
    if (!json.contains("tokens_won") || !json.contains("username"))
        return;
    json["tokens_won"].get_to(entry.tokens_won);
    json["username"].get_to(entry.username);
}


void USefaPoolChampionship::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                           const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback_get_leaderboards = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        FString json_str(resp_json.dump(2).c_str());
        UE_LOG(LogTemp, Warning, TEXT("static_info: \n%s"), *json_str)
        if (check_status(resp_json, "championship/leaderboards"))
        {
            for (int division = 1; division <= 6; division++)
            {
                if (resp_json.contains(std::to_string(division)))
                {
                    if (leaderboards.Contains(division))
                    {
                        leaderboards[division].leaderboard.Empty();
                    }
                    else
                    {
                        FChampionshipDivision div;
                        leaderboards.Add(division, div);
                    }
                    for (auto& entry : resp_json[std::to_string(division)]["leaderboard"])
                    {
                        leaderboards[division].leaderboard.Add(entry);
                    }
                    if (resp_json[std::to_string(division)].contains("position"))
                        leaderboards[division].pos = resp_json[std::to_string(division)]["position"].get<int>();
                    else
                        leaderboards[division].pos = -1;

                    if (resp_json[std::to_string(division)].contains("valid_to"))
                        leaderboards[division].endTime = FDateTime::FromUnixTimestamp(resp_json[std::to_string(division)]["valid_to"].get<int64>());
                    else
                        leaderboards[division].endTime = FDateTime::FromUnixTimestamp(0); // TODO Next Sunday
                }
                else
                    if (leaderboards.Contains(division))
                        leaderboards.Remove(division);
            }
            if (successful) successful(this);
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("championship", "leaderboards", cool_json_t(), callback_get_leaderboards);
}
