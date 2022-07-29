
#include "SefaPoolTournaments.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPool/Widgets/LoadingWidget.h"

void USefaPoolTournaments::Save()
{
    UGameplayStatics::SaveGameToSlot(this, gi->GetSaveGamePrefix() + "tournaments", 0);
}
    
void USefaPoolTournaments::Load()
{
    USefaPoolTournaments* LoadedGame = Cast<USefaPoolTournaments>(UGameplayStatics::LoadGameFromSlot(gi->GetSaveGamePrefix() + "tournaments", 0));
    if (!LoadedGame)
    {
        PRINTF((TEXT("Failed to load tournaments")), Yellow)
    }
    else
    {
        //*this = *LoadedGame;
        PRINTF((TEXT("Friends successfully loaded")), Green)
    }
}

void from_json(const cool_json_t& json, FTournamentInfo& ti)
{
    //if (json.contains("tourn_id"))
    //ti.tourn_id = json["tourn_id"].get<FString>();
    ti.difficulty = json["difficulty"].get<EDifficultyLevels>();//static_cast<EDifficultyLevels> (json["difficulty"].get<int>());
    ti.ip = json["ip"].get<FString>();
    if (LOBBY_URL != std::string("NO")) ti.ip = LOBBY_URL;
    ti.port = json["port"].get<int>();
    ti.filled = true;
}

void USefaPoolTournaments::PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                             const std::function<void(USefaPoolDataBlock*)>&& unsuccessful)
{
    if (!gi)
    {
        if (unsuccessful)
            unsuccessful(this);
        return;
    }
    //if (successful) successful(this);
    /*
    { 
      "tourns" : [ 
        { "tourn_id" : "1", 
          "ticket_price" : "24", 
          "difficulty" : "6", 
          "ip" : "127.0.0.1", 
          "port" : 12345 }
        ], 
       "status" : 200 
     }
    */
    /*auto callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "tournament/get_free_tourns"))
        {
            UE_LOG(LogTemp, Warning, TEXT("Received update from tournaments: %s"), *(FString(resp_json.dump(2).c_str())))
            TournLobbiesInfo.Empty();
            if (resp_json.contains("tourns")) {
                TournLobbiesInfo.Init(FTournamentInfo(), resp_json["tourns"].size());
                int filled = 0;
                for (const auto &tourn_info : resp_json["tourns"]) {
                    UE_LOG(LogTemp, Warning, TEXT("Received update from tournaments: %s"), *(FString(tourn_info.dump(2).c_str())))
                    tourn_info.get_to(TournLobbiesInfo[filled]);
                    filled++;
                }
                if (successful) successful(this);
            }
            else
                if (unsuccessful) unsuccessful(this);
            
        }
    };

    gi->MakeRequest("tournament", "get_free_tourns", cool_json_t(), callback);*/
    
    auto participate_callback = [this, successful, unsuccessful](cool_json_t resp_json) -> void {
        if (check_status(resp_json, "tournament/participate_free_tourn"))
        {
            if (resp_json.contains("ip") &&
                resp_json.contains("port") &&
                resp_json.contains("difficulty"))
            {
                resp_json.get_to(ti);
                if (successful) successful(this);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("tournament/participate_free_tourn did not contain ip, port or diff"))
                if (unsuccessful) unsuccessful(this);
            }
        }
        else if (unsuccessful) unsuccessful(this);
    };
    
    gi->MakeRequestWithFail("tournament",
                            "participate_free_tourn",
                            cool_json_t(),
                            participate_callback,
                            [unsuccessful, this](){if (unsuccessful) unsuccessful(this);});
}

void USefaPoolTournaments::ParticipateInFreeTournament(const FResponseDelegat& successful,
                                                       const FResponseDelegat& unsuccessful)
{
    /*if (TournLobbiesInfo.Num() > 0)
    {
        auto success_callback = [this](cool_json_t json) -> void {
            FString Options = FString("user_id=") + gi->backend_data.user_id + "?";
            Options += FString("access_token=") + gi->backend_data.access_token;

            FString level_name = TournLobbiesInfo[0].ip + ":" + FString::FromInt(TournLobbiesInfo[0].port);
            UE_LOG(LogTemp, Warning, TEXT("traveling to %s"), *(level_name));

            ULoadingWidget* wg = Cast<ULoadingWidget>(gi->widgetManager->GetWidget("Loading"));
            if (wg)
                wg->ChangeVisibleText("Connecting to tournament");
            else
                PRINTF((TEXT("Widget was nullptr")), Red);
            gi->OpenLevel(level_name, Options);
        };

        gi->MakeRequestWithFail(
            "core/internal",
            "deduct_tokens",
            {
                {"user_id", gi->backend_data.user_id},
                {"amount", TournLobbiesInfo[0].ticket_price}
            },
            success_callback,
            []() { PRINTF((TEXT("Not enough balance")), Yellow)}
        );
    }*/
    
    if (!ti.filled)
    {
        unsuccessful.Execute();
        return;
    }
    
    FString Options = FString("user_id=") + gi->backend_data.user_id + "?";
    Options += FString("access_token=") + gi->backend_data.access_token;

    FString level_name = ti.ip + ":" + FString::FromInt(ti.port);
    UE_LOG(LogTemp, Warning, TEXT("traveling to %s"), *(level_name));

    ULoadingWidget* wg = Cast<ULoadingWidget>(gi->widgetManager->GetWidget("Loading"));
    if (wg)
        wg->ChangeVisibleText("Connecting to tournament");
    else
        PRINTF((TEXT("Widget was nullptr")), Red);
    ti.filled = false;
    ti.ip.Empty();
    ti.port = 0;
    successful.Execute();
    gi->OpenLevel(level_name, Options);
}


