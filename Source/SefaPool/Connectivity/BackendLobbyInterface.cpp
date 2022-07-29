
#include "BackendLobbyInterface.h"

void UBackendLobbyInterface::Init(USefaPoolGameInstance* gi_)
{
    gi = gi_;
}

void UBackendLobbyInterface::Update()
{
    if (!gi) return;
    
    auto callback = [this](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "lobbyrelay/get_lobby"))
        {
            if (resp_json.contains("ip") && resp_json.contains("port"))
            {
                resp_json["ip"].get_to(ip);
                if (LOBBY_URL != std::string("NO")) ip = LOBBY_URL;
                resp_json["port"].get_to(port);
                UE_LOG(LogTemp, Warning, TEXT("Received lobby info: %s:%d"), *ip, port)
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("lobbyrelay/get_lobby did not contain ip or port"))
            }
        }
    };
    
    gi->MakeRequest("lobbyrelay", "get_lobby", cool_json_t(), callback);
}

FString UBackendLobbyInterface::GetLobbyTravelAddress()
{
    return ip + FString(":") + FString::FromInt(port);
}
