
#include "SefaPoolLobby.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"

void USefaPoolLobby::Save()
{
    
}
    
void USefaPoolLobby::Load()
{
    
}
    
void USefaPoolLobby::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                       const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    
    auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "lobbyrelay/get_lobby"))
        {
            if (resp_json.contains("ip") && resp_json.contains("port"))
            {
                resp_json["ip"].get_to(ip);
                if (LOBBY_URL != std::string("NO")) ip = LOBBY_URL;
                resp_json["port"].get_to(port);
                UE_LOG(LogTemp, Warning, TEXT("Received lobby info: %s:%d"), *ip, port)
                if (successful) successful(this);
            }
            else
            {
                if (unsuccessful) unsuccessful(this);
                UE_LOG(LogTemp, Warning, TEXT("lobbyrelay/get_lobby did not contain ip or port"))
            }
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequest("lobbyrelay", "get_lobby", cool_json_t(), callback);
}


FString USefaPoolLobby::GetLobbyTravelAddress()
{
    return ip + FString(":") + FString::FromInt(port);
}
