
#include "PoolRemoteMessageContestWinWidget.h"


void UPoolRemoteMessageContestWinWidget::Parse(cool_json_t json)
{
    if (!json.contains("type") ||
        json["type"] != ERemoteMessageType::CONTEST_WIN_PERSISTENT)
    {
        return;
    }
    UE_LOG(LogTemp, Warning, TEXT("Successfully parsed contest win message"));
    Init();

    /*
    message["type"] = static_cast<int>(CLIENT_MESSAGE::CONTEST_WIN);
    message["contest_id"] = contest_id;
    message["amount"] = amount;
    message["balls"] = balls;
    message["time"] = time;
    message["place"] = place;
    message["difficulty"] = difficulty;
    message["type"] = type;
    */
}
