
#include "PoolRemoteMessageFriendResponseWidget.h"


void UPoolRemoteMessageFriendResponseWidget::Parse(cool_json_t json)
{
    UE_LOG(LogTemp, Warning, TEXT("Parsing friend response json: %s"), *FString(json.dump().c_str()));
    if (!json.contains("type") ||
        json["type"] != ERemoteMessageType::FRIEND_RESPONSE_TRANSIENT ||
        !(json.contains("user_id") && json.contains("username")))
    {
        username.Empty();
        user_id.Empty();
        return;
    }
    json["user_id"].get_to(user_id);
    json["username"].get_to(username);
    UE_LOG(LogTemp, Warning, TEXT("Successfully parsed friend response message"));
    Init();
}
