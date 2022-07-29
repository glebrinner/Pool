
#include "PoolRemoteMessageChampionshipWinWidget.h"


void UPoolRemoteMessageChampionshipWinWidget::Parse(cool_json_t json)
{
    if (!json.contains("type") ||
        json["type"] != ERemoteMessageType::CHAMPIONSHIP_WIN_PERSISTENT)
    {
        return;
    }
    json["place"].get_to(place);
    UE_LOG(LogTemp, Warning, TEXT("Successfully parsed contest win message"));
    Init();
}
