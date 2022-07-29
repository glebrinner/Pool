
#include "PoolRemoteMessageFriendRequestWidget.h"
#include "PoolRemoteMessagesWidget.h"


void UPoolRemoteMessageFriendRequestWidget::Parse(cool_json_t json)
{
    if (!json.contains("type") ||
        json["type"] != ERemoteMessageType::FRIEND_REQUEST_TRANSIENT ||
        !(json.contains("user_id") && json.contains("username")))
    {
        username.Empty();
        user_id.Empty();
        return;
    }
    json["user_id"].get_to(user_id);
    json["username"].get_to(username);
    UE_LOG(LogTemp, Warning, TEXT("Successfully parsed friend request message"));
    Init();
}


void UPoolRemoteMessageFriendRequestWidget::AcceptFriendRequest()
{
    if (!parent && !parent->gi) return;
    parent->gi->data->friends->AcceptFriendRequest(user_id);
}

void UPoolRemoteMessageFriendRequestWidget::IgnoreFriendRequest()
{
    if (!parent && !parent->gi) return;
    parent->gi->data->friends->IgnoreFriendRequest(user_id);
}
