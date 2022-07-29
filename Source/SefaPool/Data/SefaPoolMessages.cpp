
#include "SefaPoolMessages.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPool/Widgets/MainMenu/RemoteMessages/PoolRemoteMessagesWidget.h"
#include "SefaPool/Shared/PoolPlayerControllerBase.h"

void from_json(const cool_json_t& json, ERemoteMessageType& mt)
{
    if (!json.contains("type"))
    {
        mt = ERemoteMessageType::NO_MESSAGE;
        return;
    }
    int type = json["type"].get<int>();
    if (type < 0 || type >= static_cast<int> (ERemoteMessageType::MESSAGE_NUM))
    {
        mt = ERemoteMessageType::NO_MESSAGE;
        return;
    }
    mt = static_cast<ERemoteMessageType> (type);
}

void to_json(cool_json_t& json, const EPlayerMenuState& pms)
{
    json = cool_json_t(static_cast<int>(pms));
}

void from_json(const cool_json_t& json, EPlayerMenuState& pms)
{
    int state = json.get<int>();
    if (state < 0 || state > static_cast<int> (EPlayerMenuState::IN_LOBBY))
    {
        pms = EPlayerMenuState::OFFLINE;
        return;
    }
    pms = static_cast<EPlayerMenuState> (state);
}

void USefaPoolMessages::Init(USefaPoolGameInstance* gi_)
{
    Super::Init(gi_);
    if (!gi) return;
    gi->TimerManager->SetTimer(pingTimer, this,
                               &USefaPoolMessages::SendPingOnTimer, 6.0f, true, 2.0f);
}

void USefaPoolMessages::Save()
{
    //UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "messages", 0);
}
    
void USefaPoolMessages::Load()
{
    /*USefaPoolMessages* LoadedGame = Cast<USefaPoolMessages>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "messages", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load messages")), Yellow)
    }
    else
    {
        *this = *LoadedGame;
        PRINTF((TEXT("Messages successfully loaded")), Green)
    }*/
}

void USefaPoolMessages::RegisterMessage(cool_json_t message)
{
    UE_LOG(LogTemp, Warning, TEXT("Registering message %s"), *FString(message.dump().c_str()))
    FRemoteMessageInfo rmi;
    rmi.messageJson = message.dump().c_str();
    rmi.shortDescription = JsonToShortMessageDescription(message);
    rmi.read = false;
    largestMessageId++;
    messages.Add(largestMessageId, rmi);
}

void USefaPoolMessages::MessageRead(int id)
{
    if (!messages.Contains(id)) return;

    messages.Find(id)->read = true;
}

void USefaPoolMessages::MessageDismissed(int id)
{
    messages.Remove(id);
}

void USefaPoolMessages::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                          const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "user/get_active_messages"))
        {
            /*std::vector<cool_json_t> fakeMessages;

            {
                cool_json_t tmp;
                tmp["type"] = ERemoteMessageType::FRIEND_REQUEST_TRANSIENT;
                tmp["user_id"] = "1";
                tmp["username"] = "1";
                fakeMessages.push_back(tmp);
            }

            {
                cool_json_t tmp;
                tmp["type"] = ERemoteMessageType::FRIEND_RESPONSE_TRANSIENT;
                tmp["user_id"] = "1";
                tmp["username"] = "1";
                fakeMessages.push_back(tmp);
            }

            {
                cool_json_t tmp;
                tmp["type"] = ERemoteMessageType::CONTEST_WIN_PERSISTENT;
                fakeMessages.push_back(tmp);
            }

            {
                cool_json_t tmp;
                tmp["type"] = ERemoteMessageType::INVITE_TO_GAME_TRANSIENT;
                tmp["invite_id"] = "awkfujbakjfb";
                tmp["user_id"] = "1";
                tmp["username"] = "1";
                tmp["difficulty"] = 5;
                tmp["amount"] = 200;
                fakeMessages.push_back(tmp);
            }

            {
                cool_json_t tmp;
                tmp["type"] = ERemoteMessageType::START_FRIENDLY_GAME_TRANSIENT;
                tmp["ip"] = "10.10.10.10";
                tmp["port"] = 5616;
                tmp["username"] = "1";
                fakeMessages.push_back(tmp);
            }

            {
                cool_json_t tmp;
                tmp["type"] = ERemoteMessageType::CHAMPIONSHIP_WIN_PERSISTENT;
                tmp["place"] = 17;
                fakeMessages.push_back(tmp);
            }

            {
                cool_json_t tmp;
                tmp["type"] = ERemoteMessageType::FREE_TOURNAMENT_WIN_TRANSIENT;
                tmp["amount"] = 5000;
                fakeMessages.push_back(tmp);
            }

            for (auto mess : fakeMessages)
            {
                RegisterMessage(mess);
            }*/

            UE_LOG(LogTemp, Warning, TEXT("    Messages: %s"), *(FString(resp_json.dump().c_str())))
            if (resp_json.contains("messages") &&
                resp_json["messages"].is_array() &&
                resp_json["messages"].size() > 0)
            {
                std::vector<cool_json_t> new_messages = resp_json["messages"];
                activeMessages.insert(activeMessages.end(), new_messages.begin(), new_messages.end());

                for (auto mess : new_messages)
                {
                    RegisterMessage(mess);
                    UE_LOG(LogTemp, Warning, TEXT("    Message: %s"), *(FString(mess.dump().c_str())))
                }

                
                
                if (successful) successful(this);
                
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                UE_LOG(LogTemp, Warning, TEXT("user/get_active_messages did not messages"))
            }
            Save();
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("user", "get_active_messages", cool_json_t(), callback);
}

void USefaPoolMessages::SendPingOnTimer()
{
    if (!gi/* || !pingOk*/)
        return;
    //pingOk = false;
    auto netMode = GetWorld()->GetNetMode();
    EPlayerMenuState state = EPlayerMenuState::IN_LOBBY;
    if (netMode == NM_Standalone)
    {
        state = EPlayerMenuState::MAIN_MENU;
    }
    else if (netMode == NM_Client)
    {
        APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
        if (pc)
        {
            state = pc->IsSpectator() ? EPlayerMenuState::SPECTATING : EPlayerMenuState::PLAYING;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("state %d"), static_cast<int>(state))
    
    auto callback = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "gamesession/update_session"))
        {
            UE_LOG(LogTemp, Warning, TEXT("    Ping: %s"), *(FString(resp_json.dump().c_str())))
            if (resp_json.contains("messages") &&
                resp_json["messages"].is_array() &&
                resp_json["messages"].size() > 0)
            {
                //std::vector<cool_json_t> new_messages = resp_json["messages"];
                //std::vector<cool_json_t> tmp_messages = activeMessages;
                //tmp_messages.insert(tmp_messages.end(), new_messages.begin(), new_messages.end());
                
                for (auto mess : resp_json["messages"])
                {
                    RegisterMessage(mess);
                }

                /*auto messagesWidget = dynamic_cast<UPoolRemoteMessagesWidget*> (gi->widgetManager->GetWidget("RemoteMessages"));
                if (messagesWidget)
                {
                    messagesWidget->Update(tmp_messages);
                }*/

                gi->widgetManager->ShowWidget("NewRemoteMessage");
                
            }
            //Save();
        }
        //pingOk = true;
    };
    
    cool_json_t req;
    req["state"] = state;
    
    gi->MakeRequestWithFail("gamesession", "update_session", req, callback, [&](){/*pingOk = true;*/});
}


FString USefaPoolMessages::JsonToShortMessageDescription(cool_json_t json)
{
    ERemoteMessageType type = ERemoteMessageType::NO_MESSAGE;
    json.get_to(type);
    FString desc;
    switch (type)
    {
        case ERemoteMessageType::NO_MESSAGE:
            return TEXT("");
        case ERemoteMessageType::FRIEND_REQUEST_TRANSIENT:
            json["username"].get_to(desc);
            desc.Append(" sent you a friend request");
            break;
        case ERemoteMessageType::FRIEND_RESPONSE_TRANSIENT:
            json["username"].get_to(desc);
            desc.Append(" accepted your friend request");
            break;
        case ERemoteMessageType::INVITE_TO_GAME_TRANSIENT:
            json["username"].get_to(desc);
            desc.Append(" invites you to play a game");
            break;
        case ERemoteMessageType::START_FRIENDLY_GAME_TRANSIENT:
            json["username"].get_to(desc);
            desc.Append(" has accepted your invite");
            break;
        case ERemoteMessageType::CONTEST_WIN_PERSISTENT:
            desc = TEXT("You won in the contest");
            break;
        case ERemoteMessageType::CHAMPIONSHIP_WIN_PERSISTENT:
            desc = TEXT("You won in the championship");
            break;
        case ERemoteMessageType::FREE_TOURNAMENT_WIN_TRANSIENT:
            desc = TEXT("You won in the tournament");
            break;
    }
    return desc;
}

FRemoteMessageInfo USefaPoolMessages::GetLatestMessage()
{
    if (messages.Contains(largestMessageId))
        return messages[largestMessageId];
    return FRemoteMessageInfo{};
}