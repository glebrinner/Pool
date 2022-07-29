// Fill out your copyright notice in the Description page of Project Settings.


#include "TournBallReferee.h"

ATournBallReferee::ATournBallReferee() : APool8BallReferee()
{
}

void ATournBallReferee::BeginPlay()
{
	Super::BeginPlay();
	
	UBackendTourn1v1Info* tourn_backend_data = Cast<UBackendTourn1v1Info>(backend_data);
	//TODO change of ip has to be on client side
	win_game_ip = tourn_backend_data->tourn_lobby_external_ip;
	win_game_port = tourn_backend_data->tourn_lobby_external_port;
	if (GS_IP_FOR_CLIENTS != std::string("NO")) win_game_ip = GS_IP_FOR_CLIENTS;
	win_game_level = FName(FString::Format(TEXT("{0}:{1}"), { win_game_ip, win_game_port }));
	lose_game_level = TEXT("Authentication");
	win_game_widget = TEXT("WinScreen");
	lose_game_widget = TEXT("LoseScreen");

    
    consolidation_event_name = TEXT("Match:tournament:end");
    consolidation_json_data["tourn_id"] = tourn_backend_data->tourn_id;

}

void ATournBallReferee::GameResults()
{
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (!gi) return;
	UBackendTourn1v1Info* tourn_backend_data = Cast<UBackendTourn1v1Info>(backend_data);
	int node_id = 1;
	if (tourn_backend_data)
		node_id = tourn_backend_data->node_id;
	else
		UE_LOG(LogTemp, Warning, TEXT("Backend data wasn't valid"));
#if UE_SERVER
    FString redis_ip = TEXT("redis");
    if (tourn_backend_data)
        redis_ip = tourn_backend_data->tournament_pod_ip;
    URedisObject* redis = NewObject<URedisObject>();
    redis->Init(redis_ip, 6379, "");
	if (redis)
	{
		cool_json_t json = {
			{"match_id", node_id},
			{"status", "ended"},
			{"win_user_id", win_user_id_},
			{"lose_user_id", lose_user_id_},
		};

		if (redis->Publish(TEXT("game_results"), FString(json.dump().c_str())))
		{
			UE_LOG(LogTemp, Warning, TEXT("Redis publishing to game_results was succesful."));
			redis->Quit();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Redis publishing to game_results wasn't succesful."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Pointer redis wasn't valid in ATournBallReferee."));
	}
#endif
    cool_json_t consolidate_json;
    consolidate_json["tourn_id"] = tourn_backend_data->tourn_id;
    consolidate_json["match_id"] = tourn_backend_data->match_id;
    consolidate_json["win_user_id"] = CreateConsolidationJsonForPlayer(win_user_id_);
    consolidate_json["lose_user_id"] = CreateConsolidationJsonForPlayer(lose_user_id_);
    consolidate_json["firm_break"] = is_firm_break;
    consolidate_json["firm_replay"] = is_firm_replay;
    if (!consolidation_json_data.empty())
        consolidate_json.update(consolidation_json_data);

    UE_LOG(LogTemp, Warning, TEXT("Win player user id: %s."), *consolidate_json["win_user_id"]["user_id"].get<FString>());
    UE_LOG(LogTemp, Warning, TEXT("Lose player user id: %s."), *consolidate_json["lose_user_id"]["user_id"].get<FString>());

    auto succesful = [](cool_json_t json) {
        if (!json.contains("status") || json["status"].get<int>() != 200)
        {
            UE_LOG(LogTemp, Warning, TEXT("Bad status on issue_match_result_from_reserved_funds."));
        }
        else
        {
        }
    };
    auto unsuccesful = []() {
        UE_LOG(LogTemp, Warning, TEXT("Request DistributeFunds wasn't succesful"));
    };
    
    cool_json_t event_json;
    event_json["event_name"] = consolidation_event_name;
    event_json["item"] = consolidate_json;

    gi->MakeRequestWithFail("eventmanager", "register_event", event_json, succesful, unsuccesful);
}
