
#include "PoolRemoteMessageInviteToGameWidget.h"
#include "PoolRemoteMessagesWidget.h"


void UPoolRemoteMessageInviteToGameWidget::Parse(cool_json_t json)
{
    if (!json.contains("type") ||
        json["type"] != ERemoteMessageType::INVITE_TO_GAME_TRANSIENT)
    {
        return;
    }
    json["invite_id"].get_to(invite_id);
    json["user_id"].get_to(user_id);
    json["username"].get_to(username);
    json["difficulty"].get_to(difficulty);
    json["amount"].get_to(amount);
    UE_LOG(LogTemp, Warning, TEXT("Successfully parsed invite to game message"));
    Init();
}

void UPoolRemoteMessageInviteToGameWidget::AcceptInvitation()
{
    if (!parent && !parent->gi) return;
    parent->gi->data->friends->AcceptInvitation(invite_id);
}

void UPoolRemoteMessageInviteToGameWidget::RejectInvitation()
{
    if (!parent && !parent->gi) return;
    parent->gi->data->friends->RejectInvitation(invite_id);
}
