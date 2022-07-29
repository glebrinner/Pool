
#include "SefaPoolStats.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolStats::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "stats", 0);
}
    
void USefaPoolStats::Load()
{
    USefaPoolStats* LoadedGame = Cast<USefaPoolStats>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "stats", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load stats")), Yellow)
    }
    else
    {
        data.games_won = LoadedGame->data.games_won;
        data.games_lost = LoadedGame->data.games_lost;
        data.firm_breaks = LoadedGame->data.firm_breaks;
        data.firm_replays = LoadedGame->data.firm_replays;
        data.reaction_time = LoadedGame->data.reaction_time;
        data.tournaments_won = LoadedGame->data.tournaments_won;
        PRINTF((TEXT("Stats successfully loaded")), Green)
    }
}
    
void USefaPoolStats::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                       const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/get_stats"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    Stats: %s"), *(FString(resp_json.dump().c_str())))
            if (resp_json.contains("stats") &&
                resp_json["stats"].contains("games_won") &&
                resp_json["stats"].contains("games_lost") &&
                resp_json["stats"].contains("firm_breaks") &&
                resp_json["stats"].contains("firm_replays") &&
                resp_json["stats"].contains("reaction_time") &&
                resp_json["stats"].contains("tournaments_won"))
            {
                resp_json["stats"]["games_won"].get_to(data.games_won);
                resp_json["stats"]["games_lost"].get_to(data.games_lost);
                resp_json["stats"]["firm_breaks"].get_to(data.firm_breaks);
                resp_json["stats"]["firm_replays"].get_to(data.firm_replays);
                resp_json["stats"]["reaction_time"].get_to(data.reaction_time);
                resp_json["stats"]["tournaments_won"].get_to(data.tournaments_won);
                if (successful) successful(this);
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                UE_LOG(LogTemp, Warning, TEXT("user/get_stats wrong json"))
            }
            Save();
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("user",
        "get_stats",
        {{
            "target_user_id",
            gi->backend_data.user_id
        }},
        callback);
}

void USefaPoolStats::PullStatsForUser(const FString& user_id,
                                      const FResponseDelegat& successful,
                                      const FResponseDelegat& unsuccessful)
{
    if (!gi)
    {
        unsuccessful.Execute();
        return;
    }
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/get_stats"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    Stats: %s"), *(FString(resp_json.dump().c_str())))
            if (resp_json.contains("stats") &&
                resp_json["stats"].contains("games_won") &&
                resp_json["stats"].contains("games_lost") &&
                resp_json["stats"].contains("firm_breaks") &&
                resp_json["stats"].contains("firm_replays") &&
                resp_json["stats"].contains("reaction_time") &&
                resp_json["stats"].contains("tournaments_won"))
            {
                resp_json["stats"]["games_won"].get_to(ext_stats.games_won);
                resp_json["stats"]["games_lost"].get_to(ext_stats.games_lost);
                resp_json["stats"]["firm_breaks"].get_to(ext_stats.firm_breaks);
                resp_json["stats"]["firm_replays"].get_to(ext_stats.firm_replays);
                resp_json["stats"]["reaction_time"].get_to(ext_stats.reaction_time);
                resp_json["stats"]["tournaments_won"].get_to(ext_stats.tournaments_won);
                successful.Execute();
            }
            else
            {
                unsuccessful.Execute();
                UE_LOG(LogTemp, Warning, TEXT("user/get_stats wrong json"))
            }
            Save();
        }
        else unsuccessful.Execute();
    };
    
    gi->MakeRequest("user",
        "get_stats",
        {{
            "target_user_id",
            user_id
        }},
        callback);
}
