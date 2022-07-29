
#include "PoolRemoteMessageStartFriendlyGameWidget.h"
#include "PoolRemoteMessagesWidget.h"


void UPoolRemoteMessageStartFriendlyGameWidget::Parse(cool_json_t json)
{
    if (!json.contains("type") ||
        json["type"] != ERemoteMessageType::START_FRIENDLY_GAME_TRANSIENT)
    {
        return;
    }
    json["ip"].get_to(ip);
    json["port"].get_to(port);
    json["username"].get_to(username);
    UE_LOG(LogTemp, Warning, TEXT("Successfully parsed contest win message"));
    Init();
}

void UPoolRemoteMessageStartFriendlyGameWidget::Travel()
{
    if (!parent && !parent->gi) return;
    FString Options = FString("user_id=") + parent->gi->backend_data.user_id + "?";
    FString level = ip + TEXT(":") + FString::FromInt(port);
    parent->gi->OpenLevel(level, Options);
}
