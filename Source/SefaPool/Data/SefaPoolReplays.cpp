
#include "SefaPoolReplays.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"
#include "Kismet/GameplayStatics.h"

void from_json(const cool_json_t& json, FReplayInfo& struct_)
{
    struct_.id_  = json["id"].get<FString>();
    struct_.room = json["room"].get<FString>();
    struct_.gm   = json["referee"].get<FString>();
    struct_.won  = json["won"].get<bool>();
    struct_.type = static_cast<EPoolGameType>(json["game_type"].get<int>());

    if (json["info"].contains("amount"))
        struct_.amount = json["amount"].get<int>();

    if (json["info"].contains("fee"))
        struct_.amount = json["fee"].get<int>();

    if (json["info"].contains("node_id"))
        struct_.tournament_node_id = json["node_id"].get<int>();

    if (json["info"].contains("difficulty"))
        struct_.diffLevel = json["difficulty"].get<EDifficultyLevels>();

    if (json["info"].contains("division"))
        struct_.diffLevel = json["division"].get<EDifficultyLevels>();

    if (json["info"].contains("contest_type"))
        struct_.contestType = json["contest_type"].get<EContestType>();

    if (json.contains("others") && json["others"].is_array() && json["others"].size() == 1)
    {
        struct_.opponentId = json["others"][0]["player_id"].get<FString>();
        struct_.opponentUsername = json["others"][0]["username"].get<FString>();
    }
}

bool FReplayInfo::operator==(const FReplayInfo& that)
{
    if (id_ == that.id_) return true;
    return false;
}

void USefaPoolReplays::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "Replays", 0);
}
    
void USefaPoolReplays::Load()
{
    USefaPoolReplays* LoadedGame = Cast<USefaPoolReplays>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "Replays", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load Replays")), Yellow)
    }
    else
    {
        replays = LoadedGame->replays;
        
        for (auto& replay : replays)
        {
            if (replay.downloadStatus == EReplayDownloadStatus::DOWNLOADING)
            {
                replay.downloadStatus = EReplayDownloadStatus::ONLINE;
                UGameplayStatics::DeleteGameInSlot(gi->GetSaveGamePrefix() + "replay_" + replay.id_, 0);
            }
        }
        
        PRINTF((TEXT("Replays successfully loaded")), Green)
    }
}
    
void USefaPoolReplays::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                         const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "replay/get_available_replays"))
        {
            UE_LOG(LogTemp, Warning, TEXT("Replays: \n%s"), *FString(resp_json.dump(2).c_str()))
            for (auto& oldReplayDesc : replays)
                oldReplayDesc.presentOnline = false;

            /*cool_json_t fake_json;
            cool_json_t fake_json_element;

            fake_json_element["room"] = "Room1";
            fake_json_element["id"] = "awgoiango";
            fake_json_element["referee"] = "SP_GM";
            fake_json_element["amount"] = 500;
            fake_json_element["fee"] = 100;
            fake_json_element["node_id"] = 2;
            fake_json_element["won"] = true;
            fake_json_element["difficulty"] = EDifficultyLevels::DIFFICULTY_LEVEL_3;
            fake_json_element["contest_type"] = EContestType::CONTEST_LINE_UP;
            cool_json_t fake_opponent;
            fake_opponent["player_id"] = "AFAG";
            fake_opponent["username"] = "SomePlayer";
            fake_json_element["others"].push_back(fake_opponent);

            fake_json_element["game_type"] = EPoolGameType::GAME_1v1;
            fake_json.push_back(cool_json_t(fake_json_element));
            fake_json_element["id"] = "awgoiango1";
            fake_json_element["game_type"] = EPoolGameType::GAME_CONTEST;
            fake_json.push_back(cool_json_t(fake_json_element));
            fake_json_element["id"] = "awgoiango12";
            fake_json_element["game_type"] = EPoolGameType::TOURN_GAME_1v1;
            fake_json.push_back(cool_json_t(fake_json_element));
            fake_json_element["id"] = "awgoiango13";
            fake_json_element["game_type"] = EPoolGameType::FRIENDLY_GAME_1v1;
            fake_json.push_back(cool_json_t(fake_json_element));
            fake_json_element["id"] = "awgoiango14";
            fake_json_element["won"] = false;
            fake_json_element["game_type"] = EPoolGameType::GAME_1v1;
            fake_json_element["id"] = "awgoiango15";
            fake_json.push_back(cool_json_t(fake_json_element));
            fake_json_element["game_type"] = EPoolGameType::TOURN_GAME_1v1;
            fake_json_element["id"] = "awgoiango16";
            fake_json.push_back(cool_json_t(fake_json_element));
            fake_json_element["game_type"] = EPoolGameType::FRIENDLY_GAME_1v1;
            fake_json_element["id"] = "awgoiango17";
            fake_json.push_back(cool_json_t(fake_json_element));*/

            for (auto& replayDesc : /*fake_json*/resp_json["replays"])
            {
                FReplayInfo new_replay;
                replayDesc.get_to(new_replay);
                bool found = false;
                for (auto& oldReplayDesc : replays)
                {
                    if (oldReplayDesc == new_replay)
                    {
                        found = true;
                        oldReplayDesc.presentOnline = true;
                        break;
                    }
                }
                if (!found)
                {
                    new_replay.downloadStatus = EReplayDownloadStatus::ONLINE;
                    new_replay.personal = true;
                    new_replay.presentOnline = true;
                    replays.Add(new_replay);
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("Received replays: %d\n"), replays.Num())
            for (int i = 0; i < replays.Num(); i++)
            {
                if (!replays[i].presentOnline &&
                    replays[i].downloadStatus != EReplayDownloadStatus::OFFLINE)
                {
                    UGameplayStatics::DeleteGameInSlot(gi->GetSaveGamePrefix() + "replay_" + replays[i].id_, 0);
                    replays.RemoveAt(i);
                    i--;
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("Received replays after cleanup: %d\n"), replays.Num())
            Save();
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("replay", "get_available_replays", cool_json_t(), callback);
}

void USefaPoolReplays::DownloadReplay(FString replayId)
{
    if (!gi)
        return;
    bool found = false;
    FReplayInfo* ri = nullptr;
    for (auto& r : replays)
    {
        if (r.id_ == replayId)
        {
            if (r.downloadStatus != EReplayDownloadStatus::ONLINE)
            {
                return;
            }
            ri = &r;
            found = true;
            break;
        }
    }
    if (!found)
        return;
    
    if (ri->personal)
    {
        cool_json_t replay_req;
        replay_req["replay_id"] = replayId;
        auto callback = [this, replayId](cool_json_t resp_json) -> void {
            if (check_status(resp_json, "replay/download_replay"))
            {
                UE_LOG(LogTemp, Warning, TEXT("Received replay: \n%s"), *replayId)
                FString replay_data = resp_json["data"].get<FString>();
                ProcessDownloadedReplay(replayId, replay_data);
            }
        };
        ri->downloadStatus = EReplayDownloadStatus::DOWNLOADING;
        gi->MakeRequestWithFailAndProgress(
            "replay",
            "download_replay",
            replay_req,
            callback,
            [](){},
            [=](float progress) {
                ri->downloadProgress = progress;
            }
        );
    }
    else
    {
        cool_json_t replay_req;
        replay_req["replay_id"] = replayId;
        auto callback = [this, replayId](cool_json_t resp_json) -> void {
            if (check_status(resp_json, "replay/download_any_replay"))
            {
                UE_LOG(LogTemp, Warning, TEXT("Received non-personal replay: \n%s"), *replayId)
                FString replay_data = resp_json["data"].get<FString>();
                ProcessDownloadedReplay(replayId, replay_data);
            }
        };
        ri->downloadStatus = EReplayDownloadStatus::DOWNLOADING;
        gi->MakeRequestWithFailAndProgress(
            "replay",
            "download_any_replay",
            replay_req,
            callback,
            [](){},
            [=](float progress) {
                ri->downloadProgress = progress;
            }
        );
    }
}

void USefaPoolReplays::ProcessDownloadedReplay(FString id_, const FString& data)
{
    bool found = false;
    FReplayInfo* ri = nullptr;
    for (auto& r : replays)
    {
        if (r.id_ == id_)
        {
            if (r.downloadStatus != EReplayDownloadStatus::DOWNLOADING)
            {
                return;
            }
            ri = &r;
            found = true;
            break;
        }
    }
    if (!found)
        return;
    
    TArray<uint8> replay_data_binary;
    FBase64::Decode(data, replay_data_binary);
    
    if (UReplaySaveGame* SaveGameInstance =
        Cast<UReplaySaveGame>(UGameplayStatics::LoadGameFromMemory(replay_data_binary)))
    {
        if (UGameplayStatics::SaveGameToSlot(SaveGameInstance,
                                             gi->GetSaveGamePrefix() + "replay_" + id_,
                                             0))
        {
            ri->downloadStatus = EReplayDownloadStatus::OFFLINE;
            Save();
        }
    }
}

FReplayInfo USefaPoolReplays::GetReplayInfoForId(FString id_)
{
    bool found = false;
    FReplayInfo* ri = nullptr;
    for (auto& r : replays)
    {
        if (r.id_ == id_)
        {
            ri = &r;
            found = true;
            break;
        }
    }
    if (!found)
    {
        FReplayInfo tmp = {};
        return tmp;
    }
    return *ri;
}

bool USefaPoolReplays::FillReplayFilename(FString id_)
{
    FReplayInfo ri = GetReplayInfoForId(id_);
    if (ri.id_.Len() > 0 &&
        ri.downloadStatus == EReplayDownloadStatus::OFFLINE)
    {
        replayFileToPlay = gi->GetSaveGamePrefix() + "replay_" + id_;
        return true;
    }
    return false;
}

TArray<FReplayInfo> USefaPoolReplays::GetReplaysFiltered(FString filter)
{
    if (filter.Len() == 0)
        return replays;

    TArray<FReplayInfo> ret;

    for (auto& r : replays)
    {
        FString description;
        description += r.opponentUsername;
        description += TEXT("$;;$");
        switch (r.type)
        {
        case EPoolGameType::GAME_1v1:
            description += TEXT("versus");
            break;
        case EPoolGameType::GAME_CONTEST:
            description += TEXT("contest");
            description += TEXT("$;;$");
            switch (r.contestType)
            {
            case EContestType::CONTEST_CIRCLE_OF_BALLS:
                description += TEXT("circle of balls");
                break;
            case EContestType::CONTEST_CUSHION:
                description += TEXT("cushion");
                break;
            case EContestType::CONTEST_FBNL:
                description += TEXT("firm break no limit");
                break;
            case EContestType::CONTEST_LINE_UP:
                description += TEXT("line up");
                break;
            case EContestType::CONTEST_SPEED_POOL:
                description += TEXT("speed pool");
                break;
            }
            break;
        case EPoolGameType::TOURN_GAME_1v1:
            description += TEXT("tournament");
            break;
        case EPoolGameType::FRIENDLY_GAME_1v1:
            description += TEXT("friendly");
            break;
        }
        description += TEXT("$;;$");
        if (description.Contains(filter))
            ret.Add(r);
    }
    return ret;
}