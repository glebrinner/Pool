
#include "SefaPoolSpectating.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void from_json(const cool_json_t& json, FSpectatableMatchInfo& struct_)
{
    if (!json.contains("ip") ||
        !json.contains("port") ||
        !json.contains("user1") ||
        !json.contains("user2"))
    {
        struct_.valid = false;
        return;
    }
    //struct_.ip      = json["ip"].get<FString>();
    if (GS_IP_FOR_CLIENTS != std::string("NO")) struct_.ip = GS_IP_FOR_CLIENTS;
    struct_.port      = json["port"].get<FString>();
    struct_.username1 = json["user1"].get<FString>();
    struct_.username2 = json["user2"].get<FString>();
    struct_.valid = true;
}

void USefaPoolSpectating::Save()
{
}
    
void USefaPoolSpectating::Load()
{
}
    
void USefaPoolSpectating::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                            const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        current_spectatable_matches.Empty();
        if (check_status(resp_json, "core/get_current_matches"))
        {
            UE_LOG(LogTemp, Warning, TEXT("Spectating: %s"), *FString(resp_json.dump(2).c_str()))
            if (resp_json.contains("matches") &&
                resp_json["matches"].is_array())
            {
                for (auto& matchJson : resp_json["matches"])
                {
                    FSpectatableMatchInfo newMatch = {};
                    matchJson.get_to(newMatch);
                    if (newMatch.valid)
                        current_spectatable_matches.Add(newMatch);
                }
                if (successful) successful(this);
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                UE_LOG(LogTemp, Warning, TEXT("core/get_current_matches did not contain birthday, first_name, second_name or username"))
            }
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("core", "get_current_matches", cool_json_t(), callback);
}


TArray<FSpectatableMatchInfo> USefaPoolSpectating::GetMatchesSortedAndFiltered(bool onlyFriends, 
    bool byTime, 
    bool ascending,
    FString filter)
{
    TArray<FSpectatableMatchInfo> filtered_matches;
    /*FSpectatableMatchInfo info1 = {TEXT(""), TEXT(""), TEXT("Hello"), TEXT("World")};
    FSpectatableMatchInfo info2 = { TEXT(""), TEXT(""), TEXT("User1"), TEXT("User2") };
    TArray<FSpectatableMatchInfo> current_spectatable_matches_;
    current_spectatable_matches_.Add(info1);
    current_spectatable_matches_.Add(info2);*/
    for (auto& match : current_spectatable_matches)
    {
        if ((filter.IsEmpty() || 
            match.username1.Contains(filter) ||
            match.username2.Contains(filter)) && 
            (!onlyFriends ||
            gi->data->friends->IsFriend(match.username1) ||
            gi->data->friends->IsFriend(match.username2)))
            filtered_matches.Add(match);
    }
    if (!byTime)
        filtered_matches.Sort([ascending = ascending](const FSpectatableMatchInfo& a, const FSpectatableMatchInfo& b) {
        bool result = (a.username1 < b.username1 || (a.username1 == b.username1 && a.username2 < b.username2));
            return ascending ? result : !result;
        });
    if (byTime && !ascending)
        Algo::Reverse(filtered_matches);
    return filtered_matches;
}
