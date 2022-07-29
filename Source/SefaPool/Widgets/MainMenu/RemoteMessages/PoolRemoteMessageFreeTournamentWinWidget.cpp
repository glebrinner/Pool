
#include "PoolRemoteMessageFreeTournamentWinWidget.h"


void UPoolRemoteMessageFreeTournamentWinWidget::Parse(cool_json_t json)
{
    if (!json.contains("type") ||
        json["type"] != ERemoteMessageType::FREE_TOURNAMENT_WIN_TRANSIENT)
    {
        return;
    }
    json["amount"].get_to(amount);
    UE_LOG(LogTemp, Warning, TEXT("Successfully parsed contest win message"));
    Init();
}
