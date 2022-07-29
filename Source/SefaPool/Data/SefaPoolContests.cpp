
#include "SefaPoolContests.h"
#include <utility>
#include "SefaPool/Widgets/LoadingWidget.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"


void from_json(const cool_json_t& json, FTrainingContestInfo& struct_)
{
    struct_.type_id =  static_cast<EContestType>(json["type"].get<int>());
    struct_.instance_id = json["id"].get<int>();
    struct_.fee = json["fee"].get<int>();
    struct_.division = json["division"].get<int>();
    struct_.ends_localTime = FDateTime::Now() + FTimespan(0, 0, json["seconds"].get<int>());
    struct_.has_user_participated = false;
}

void from_json(const cool_json_t& json, FTrainingContestResult& struct_)
{
    READ_FROM_JSON_STRING(username)
    READ_FROM_JSON_NUM(time)
    READ_FROM_JSON_NUM(balls)
}

void USefaPoolContests::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "contests", 0);
}
    
void USefaPoolContests::Load()
{
    USefaPoolContests* LoadedGame = Cast<USefaPoolContests>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "contests", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load contests")), Yellow)
    }
    else
    {
        active_contests_ = LoadedGame->active_contests_;
        active_contests_processing = LoadedGame->active_contests_processing;
        PRINTF((TEXT("Contests successfully loaded")), Green)
    }
}

void USefaPoolContests::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                          const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    active_contests_processing.Empty();
    Update_RequestActiveContests(std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (successful),
                                 std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (unsuccessful));
}

void USefaPoolContests::Update_RequestActiveContests(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                                     const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "contest/get_active_contests"))
        {
            for (const auto& active_contest_single : resp_json["contests"])
            {
                FTrainingContestInfo info = {};
                active_contest_single.get_to(info);
                active_contests_processing.Add(info.instance_id, info);
            }
            
            targeted_async_endpoints  = 3;
            completed_async_endpoints = 0;
            Update_RequestShortLeaderboards(std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (successful),
                                            std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (unsuccessful));
            Update_RequestUserResults(std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (successful),
                                      std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (unsuccessful));
            Update_RequestBestResults(std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (successful),
                                      std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (unsuccessful));
        }
        else if (unsuccessful)
            unsuccessful(this);
    };
    
    gi->MakeRequest("contest", "get_active_contests", cool_json_t(), callback);
}

void USefaPoolContests::Update_RequestShortLeaderboards(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                                        const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "contest/get_short_leaderboards"))
        {
            for (const auto& leaderboard : resp_json["leaderboards"])
            {
                int instance_id = leaderboard["id"].get<int>();
                FTrainingContestInfo* info = active_contests_processing.Find(instance_id);
                if (!info) continue;
                
                for (const auto& leaderboard_entry_json : leaderboard["results"])
                {
                    FTrainingContestResult entry = {};
                    leaderboard_entry_json.get_to(entry);
                    info->best_instance_results.Add(entry);
                }
            }
            Update_Finalize(std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (successful),
                            std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (unsuccessful));
        }
        else if (unsuccessful)
            unsuccessful(this);
    };
    cool_json_t req;
    req["N"] = 5;
    
    gi->MakeRequest("contest", "get_short_leaderboards", req, callback);
}

void USefaPoolContests::Update_RequestUserResults(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                                  const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "contest/get_user_results"))
        {
            for (const auto& single_result : resp_json["results"])
            {
                int instance_id = single_result["id"].get<int>();
                FTrainingContestInfo* info = active_contests_processing.Find(instance_id);
                if (!info) continue;
                info->has_user_participated = true;
                single_result.get_to(info->user_best_result);
            }
            Update_Finalize(std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (successful),
                            std::forward<const std::function<void(USefaPoolDataBlock*)>&&> (unsuccessful));
        }
        else if (unsuccessful)
            unsuccessful(this);
    };
    
    gi->MakeRequest("contest", "get_user_results", cool_json_t(), callback);
}

void USefaPoolContests::Update_RequestBestResults(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                                  const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "contest/get_best_results"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    CONTESTS (WR): %s"), *(FString(resp_json.dump(2).c_str())))
            for (const auto& single_result : resp_json["results"])
            {
                EContestType type_id =  static_cast<EContestType>(single_result["type"].get<int>()/1000);
                int division = single_result["type"].get<int>()%1000;

                for (auto& contest_info : active_contests_processing)
                {
                    if (type_id != contest_info.Value.type_id || 
                        contest_info.Value.division != static_cast<uint8> (division)) continue;
                    single_result.get_to(contest_info.Value.world_record);
                    UE_LOG(LogTemp, Warning, TEXT("    CONTESTS %d (WR0): time %d"), contest_info.Value.instance_id, contest_info.Value.world_record.time)
                }
            }
            Update_Finalize(std::forward<const std::function<void(USefaPoolDataBlock*)>&&>(successful),
                std::forward<const std::function<void(USefaPoolDataBlock*)>&&>(unsuccessful));

        }
        else if (unsuccessful)
            unsuccessful(this);
    };
    
    gi->MakeRequest("contest", "get_best_results", cool_json_t(), callback);
}

void USefaPoolContests::Update_Finalize(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                        const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    completed_async_endpoints++;
    if (completed_async_endpoints != targeted_async_endpoints) return;
    for (auto contest_info : active_contests_processing)
    {
        UE_LOG(LogTemp, Warning, TEXT("    CONTESTS %d (WR0.5): time %d"), contest_info.Value.instance_id, contest_info.Value.world_record.time)
    }
    
    if (successful) successful(this); 

    UE_LOG(LogTemp, Warning, TEXT("    CONTESTS DONE"))

    active_contests_.Empty();
    active_contests_processing.GenerateValueArray(active_contests_);
    for (auto contest_info : active_contests_)
    {
        UE_LOG(LogTemp, Warning, TEXT("    CONTESTS %d (WR1): time %d"), 
            contest_info.instance_id,
            contest_info.world_record.time)
    }
    Save();
}

FTrainingContestInfo USefaPoolContests::GetActiveContestByType(EContestType type,
                                                     int division)
{
    if (!this || active_contests_.Num() == 0) return {};
    
    for (const auto& contest : active_contests_)
    {
        if (contest.division == division &&
            contest.type_id == static_cast<EContestType> (type))
        {
            UE_LOG(LogTemp, Warning, TEXT("    CONTESTS %d (WR): time %d"), contest.instance_id, contest.world_record.time)
            return contest;
        }
            
    }
    return {};
}

void USefaPoolContests::Participate(int id_)
{
    if (!gi) return;
    auto callback = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "contest/start_game"))
        {
            
            FString Options = FString("user_id=") + gi->backend_data.user_id + "?"; 
            if (GS_IP_FOR_CLIENTS != std::string("NO")) resp_json["server"]["ip"] = GS_IP_FOR_CLIENTS;
            FString level_name = resp_json["server"]["ip"].get<FString>() +
                                     ":" +
                                     FString::FromInt(resp_json["server"]["port"].get<int>());
            UE_LOG(LogTemp, Warning, TEXT("traveling to %s"), *(level_name));
            
            ULoadingWidget* wg = Cast<ULoadingWidget>(gi->widgetManager->GetWidget("Loading"));
            if (wg)
            {
                wg->ChangeVisibleText("Preparing contest room ...");
            }
            else
                PRINTF((TEXT("Widget was nullptr")), Red);
            
            gi->OpenLevel(level_name, Options);
        }
    };
    cool_json_t req;
    req["contest_id"] = id_;
    
    gi->MakeRequest("contest", "start_game", req, callback);
}
