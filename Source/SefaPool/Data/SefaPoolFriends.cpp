
#include "SefaPoolFriends.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPoolMessages.h"

void USefaPoolFriends::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "friends", 0);
}
    
void USefaPoolFriends::Load()
{
    USefaPoolFriends* LoadedGame = Cast<USefaPoolFriends>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "friends", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load friends")), Yellow)
    }
    else
    {
        friendsArray = LoadedGame->friendsArray;
        PRINTF((TEXT("Friends successfully loaded")), Green)
    }
}
// {"friends":[{"id":"11","online":false,"state":-1,"username":"102"}],"status":200}
void from_json(const cool_json_t& json, FPlayerFriend& pf)
{
    if (!json.contains("username") || !json.contains("id") ||
        !json.contains("online") || !json.contains("state"))
        return;
    json["username"].get_to(pf.username);
    json["id"].get_to(pf.user_id);
    json["online"].get_to(pf.online);
    if (pf.online)
        json["state"].get_to(pf.state);
    else
        pf.state = EPlayerMenuState::OFFLINE;
        
}
    
void USefaPoolFriends::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                         const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/get_friends"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    Friends: %s"), *(FString(resp_json.dump().c_str())))
            if (resp_json.contains("friends") &&
                resp_json["friends"].is_array() &&
                resp_json["friends"].size() > 0)
            {
                friendsArray.Empty();
                for (auto friend_ : resp_json["friends"])
                {
                    if (!friend_.contains("id") ||
                        !friend_.contains("username"))
                        continue;
                    FPlayerFriend addFriend;
                    friend_.get_to(addFriend);
                    friendsArray.Add(addFriend);
                }
                if (successful) successful(this);
                Save();
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                UE_LOG(LogTemp, Warning, TEXT("user/get_all_messages did not messages"))
            }
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("user", "get_friends", cool_json_t(), callback);
}

void USefaPoolFriends::AcceptFriendRequest(FString user_id)
{
    if (!gi) return;
    
    cool_json_t req_json;
    req_json["resp"] = "accept";
    req_json["user_req_id"] = user_id;
    
    auto callback = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/friend_resp"))
        {
            UE_LOG(LogTemp, Warning, TEXT("user/friend_resp accept successful"))
        }
    };
    
    gi->MakeRequest("user", "friend_resp", req_json, callback);
}

void USefaPoolFriends::IgnoreFriendRequest(FString user_id)
{
    if (!gi) return;
    
    cool_json_t req_json;
    req_json["resp"] = "ignore";
    req_json["user_req_id"] = user_id;
    
    auto callback = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/friend_resp"))
        {
            UE_LOG(LogTemp, Warning, TEXT("user/friend_resp ignore successful"))
        }
    };
    
    gi->MakeRequest("user", "friend_resp", req_json, callback);
}

void USefaPoolFriends::SendFriendRequest(FString user_id)
{
    if (!gi) return;
    
    cool_json_t req_json;
    req_json["user_resp_id"] = user_id;
    
    auto callback = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/friend_req"))
        {
            UE_LOG(LogTemp, Warning, TEXT("user/friend_req successful"))
        }
    };
    
    gi->MakeRequest("user", "friend_req", req_json, callback);
}


void USefaPoolFriends::AcceptInvitation(FString invite_id)
{
    if (!gi) return;
    
    cool_json_t req_json;
    req_json["invite_id"] = invite_id;
    
    auto callback = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "gamesession/accept_invitation"))
        {
            UE_LOG(LogTemp, Warning, TEXT("gamesession/accept_invitation ignore successful"))
        }
    };
    
    gi->MakeRequest("gamesession", "accept_invitation", req_json, callback);
}

void USefaPoolFriends::RejectInvitation(FString invite_id)
{
    return;
}

void USefaPoolFriends::SendInvitation(FString username,
                                      EDifficultyLevels diff,
                                      int amount,
                                      const FResponseDelegat& invited,
                                      const FResponseDelegat& offline,
                                      const FResponseDelegat& not_found,
                                      const FResponseDelegat& unsuccessful)
{
    if (!gi) return;
    
    cool_json_t req_json;
    req_json["invited_username"] = username;
    req_json["difficulty"] = diff;
    req_json["amount"] = amount;
    
    auto callback = [this, invited, offline, not_found, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "gamesession/invite_to_game"))
        {
            if (resp_json.contains("found") && !resp_json["found"].get<bool>())
                not_found.Execute();
            else if (resp_json.contains("found"))
            {
                if (resp_json["is_invited"].get<bool>())
                    invited.Execute();
                else
                    offline.Execute();
            }
            else
                unsuccessful.Execute();
            
        }
        else
        {
            unsuccessful.Execute();
        }
        
    };
    
    gi->MakeRequest("gamesession", "invite_to_game", req_json, callback);
}


bool USefaPoolFriends::IsFriend(FString username)
{
    for (auto& friend_ : friendsArray)
    {
        if (friend_.username == username) return true;
    }
    return false;
}


TArray<FPlayerFriend> USefaPoolFriends::GetFriendsFiltered(FString filter, bool online)
{
    TArray<FPlayerFriend> ret;

    for (auto& friend_ : friendsArray)
    {
        if (online != friend_.online) continue;
        if (filter.IsEmpty() || friend_.username.Contains(filter))
            ret.Add(friend_);
    }
    return ret;
}