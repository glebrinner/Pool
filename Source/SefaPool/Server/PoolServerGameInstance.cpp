// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolServerGameInstance.h"
#include <string>


void UPoolServerGameInstance::Init() {
    Super::Init();
    UE_LOG(LogTemp, Display, TEXT("Initializing GI"));
}

// Macros for easy initialization of enums with int.
// Params:
// key (FString) - key in annotations map
// value (enum or int) - value that should be initialized
// type (type of value) - type of value that should be initialized. For example int or BackendPlayerInfo
// is_valid (std::function<bool(int)>) function that checks whether value with given key is valid or not
// action - code that should be used when loading is failed
#define LOAD_INT_ACTION_IF_FAIL(key, value, type, is_valid, action) \
{                                                                   \
    const FString* value_pointer = annotations.Find(key);           \
    if (!value_pointer) {                                           \
        action                                                      \
    }                                                               \
    else {                                                          \
        auto is_valid_func = is_valid;                              \
        int int_value = FCString::Atoi(**value_pointer);            \
        if (is_valid(int_value)) {                                  \
            value = static_cast<type>(int_value);                   \
            UE_LOG(LogTemp, Error, TEXT("Load key %s, with value %d"), *(FString(key)), int_value);\
        }                                                           \
        else {                                                      \
            action                                                  \
        }                                                           \
    }                                                               \
}

#define LOAD_STRING_ACTION_IF_FAIL(key, value, action)    \
{                                                         \
    const FString* value_pointer = annotations.Find(key); \
    if (!value_pointer)                                   \
    {                                                     \
        action                                            \
    }                                                     \
    UE_LOG(LogTemp, Error, TEXT("Load key %s, with value %s"), *(FString(key)), **value_pointer);\
    value = *value_pointer;                               \
}

#define PRINT_AND_RETURN(key)                                                                                       \
{                                                                                                                   \
   UE_LOG(LogTemp, Error, TEXT("ServerGameInstance: error while handling key %s in annotations"), *(FString(key))); \
   return false;                                                                                                    \
}

#define ONLY_PRINT(key)                                                                                             \
{                                                                                                                   \
   UE_LOG(LogTemp, Error, TEXT("ServerGameInstance: error while handling key %s in annotations"), *(FString(key))); \
}

#define LOAD_INT_RETURN(key, value, type, is_valid) LOAD_INT_ACTION_IF_FAIL(key, value, type, is_valid, PRINT_AND_RETURN(key))
#define LOAD_INT(key, value, type, is_valid) LOAD_INT_ACTION_IF_FAIL(key, value, type, is_valid, ONLY_PRINT(key))
#define LOAD_STRING_RETURN(key, value) LOAD_STRING_ACTION_IF_FAIL(key, value, PRINT_AND_RETURN(key))
#define LOAD_STRING(key, value) LOAD_STRING_ACTION_IF_FAIL(key, value, ONLY_PRINT(key))


bool FillPlayerInfo(const TMap<FString, FString>& annotations, FBackendPlayerInfo* players_info, int num_of_players)
{
    for (int i = 0; i < num_of_players; ++i, players_info += i)
    {
        FString usr = TEXT("user") + FString::FromInt(i + 1) + TEXT("_");
        LOAD_STRING_RETURN(usr + TEXT("id"), players_info->user_id);
        LOAD_STRING_RETURN(usr + TEXT("username"), players_info->username);
        LOAD_INT_RETURN(usr + TEXT("active_cue"), players_info->cur_cue, int, [](int cueNum) {return cueNum > 0 && cueNum <= PREDEFINED_CUES_N; });
        LOAD_INT_RETURN(usr + TEXT("active_chalk"), players_info->cur_chalk, int, [](int cueNum) {return cueNum > 0 && cueNum <= PREDEFINED_CHALKS_N; });

        for (int cueN = 1; cueN <= PREDEFINED_CUES_N; ++cueN)
        {
            LOAD_INT(usr + TEXT("cues_") + FString::FromInt(cueN), players_info->durability.cue_durabilities[cueN], int,
                [](int cueHP) {
                    return cueHP >= 0;
                })
        }
        for (int chalkN = 1; chalkN <= PREDEFINED_CHALKS_N; ++chalkN)
        {
            LOAD_INT(usr + TEXT("chalks_") + FString::FromInt(chalkN), players_info->durability.chalk_durabilities[chalkN], int,
                [](int chalkHP) {
                    return chalkHP >= 0;
                })
        }
    }
    return true;
}

EPoolGameType UBaseBackendInfo::GetGameType()
{
    return EPoolGameType::LAST_GAME_TYPE;
}

EPoolGameType UBackendContestInfo::GetGameType()
{
    return EPoolGameType::GAME_CONTEST;
}

EPoolGameType UBaseBackend1v1Info::GetGameType()
{
    return EPoolGameType::GAME_1v1;
}

EPoolGameType UBackend1v1Info::GetGameType()
{
    return EPoolGameType::GAME_1v1;
}

EPoolGameType UBackendFriendly1v1Info::GetGameType()
{
    return EPoolGameType::FRIENDLY_GAME_1v1;
}

EPoolGameType UBackendTourn1v1Info::GetGameType()
{
    return EPoolGameType::TOURN_GAME_1v1;
}

EPoolGameType UBackendTournamentInfo::GetGameType()
{
    return EPoolGameType::TOURNAMENT;
}

cool_json_t UBaseBackendInfo::GetInfoBundleJson()
{
    return cool_json_t();
}

cool_json_t UBackendContestInfo::GetInfoBundleJson()
{
    cool_json_t res;
    res["type"] = GetGameType();
    res["division"] = division;
    res["contest_type"] = type_id;
    res["fee"] = fee;
    return res;
}

cool_json_t UBaseBackend1v1Info::GetInfoBundleJson()
{
    return cool_json_t();
}

cool_json_t UBackend1v1Info::GetInfoBundleJson()
{
    cool_json_t res;
    res["type"] = GetGameType();
    res["amount"] = amount;
    res["match_id"] = match_id;
    res["difficulty"] = difficulty;
    return res;
}

cool_json_t UBackendFriendly1v1Info::GetInfoBundleJson()
{
    cool_json_t res;
    res["type"] = GetGameType();
    res["amount"] = amount;
    res["match_id"] = match_id;
    res["difficulty"] = difficulty;
    return res;
}

cool_json_t UBackendTourn1v1Info::GetInfoBundleJson()
{
    cool_json_t res;
    res["type"] = GetGameType();
    res["node_id"] = node_id;
    res["match_id"] = match_id;
    res["difficulty"] = difficulty;
    return res;
}



FBackendPlayerInfo* UBaseBackend1v1Info::GetPlayerInfo(FString user_id)
{
    if (user_id == player_info[0].user_id)
    {
        return player_info;
    }
    else if (user_id == player_info[1].user_id)
    {
        return player_info + 1;
    }
    UE_LOG(LogTemp, Warning, TEXT("Can't find user_id in GetPlayerInfo"));
    return nullptr;
}

bool UBaseBackendInfo::InitStruct(const TMap<FString, FString>& annotations)
{return true;}


FBackendPlayerInfo* UBaseBackendInfo::GetPlayerInfo(FString user_id)
{return nullptr;}

bool UBaseBackend1v1Info::InitStruct(const TMap<FString, FString>& annotations)
{    
    LOAD_INT_RETURN("difficulty", difficulty, EDifficultyLevels,
        [](int value) {
    return value >= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_1) && value <= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_6);
    });
    LOAD_INT_RETURN("match_id", match_id, int, [](int value) {return value >= 0; });
    if (!FillPlayerInfo(annotations, player_info, 2)) {
        return false;
    }
    return true;
}

bool UBackend1v1Info::InitStruct(const TMap<FString, FString>& annotations)
{
    LOAD_INT_RETURN("amount", amount, int, [](int value) {return value >= 0; });

    if (!Super::InitStruct(annotations)) {
        return false;
    }

    map = TEXT("Dark");
    game_mode = TEXT("MP_8Ball");
    return true;
}

bool UBackendFriendly1v1Info::InitStruct(const TMap<FString, FString>& annotations)
{
    return Super::InitStruct(annotations);
}

bool UBackendTourn1v1Info::InitStruct(const TMap<FString, FString>& annotations)
{
    LOAD_INT_RETURN("tourn_id", tourn_id, int, [](int value) {return value >= 0; });
    LOAD_INT_RETURN("node_id", node_id, int, [](int value) {return value >= 0; });
    LOAD_STRING_RETURN("tourn_lobby_internal_ip", tournament_pod_ip);
    LOAD_STRING_RETURN("tourn_lobby_external_ip", tourn_lobby_external_ip);
    LOAD_INT_RETURN("tourn_lobby_external_port", tourn_lobby_external_port, int, [](int value) {return value > 0 && value < 1<<16; });
    if (!Super::InitStruct(annotations)) {
        return false;
    }

    map = TEXT("TourneyRoom");
    game_mode = TEXT("MP_Tourn");
    return true;
}

bool UBackendContestInfo::InitStruct(const TMap<FString, FString>& annotations)
{
    LOAD_INT_RETURN("contest_id", contest_id, int, [](int value) { return value >= 0; });
    LOAD_INT_RETURN("division", division, EDifficultyLevels,
        [](int value) {
            return value >= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_NONE) &&
                value <= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_6);
    });
    LOAD_INT_RETURN("type_id", type_id, EContestType,
        [](int value) {
            return value > static_cast<int>(EContestType::CONTEST_NONE) &&
                value <= static_cast<int>(EContestType::CONTEST_CUSHION);
    });
    LOAD_INT_RETURN("fee", fee, int, [](int value) { return value >= 0; });
    
    if (!FillPlayerInfo(annotations, &player_info, 1)) {
        return false;
    }

    map = TEXT("Dark");

    switch (type_id)
    {
    case EContestType::CONTEST_SPEED_POOL:
        game_mode = TEXT("C_SpeedPool");
        map = TEXT("Room1");
        break;
    case EContestType::CONTEST_LINE_UP:
        game_mode = TEXT("C_LineUp");
        map = TEXT("Room3");
        break;
    case EContestType::CONTEST_FBNL:
        game_mode = TEXT("C_FBNL");
        map = TEXT("Room4");
        break;
    case EContestType::CONTEST_CIRCLE_OF_BALLS:
        game_mode = TEXT("C_CircleOfBalls");
        map = TEXT("Room5");
        break;
    case EContestType::CONTEST_CUSHION:
        game_mode = TEXT("C_Cushion");
        map = TEXT("Room6");
        break;
    }
    return true;
}

FBackendPlayerInfo* UBackendContestInfo::GetPlayerInfo(FString user_id)
{
    if (user_id == player_info.user_id)
        return &player_info;
    return nullptr;
}

bool UBackendTournamentInfo::InitStruct(const TMap<FString, FString>& annotations)
{
    LOAD_INT_RETURN("tourn_id", tourn_id, int, [](int value) {return value >= 0; });
    //Size has to be > 0 and power of two
    LOAD_INT_RETURN("size", size, int, [](int value) {return value > 0 && !(value & (value - 1)); });
    LOAD_INT_RETURN("difficulty", difficulty, EDifficultyLevels,
        [](int value) {
    return value >= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_NONE) &&
        value <= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_6);
    });
    
    map = TEXT("TournamentMap");
    game_mode = TEXT("TournamentLobby");
    return true;
}

bool UPoolServerGameInstance::ParseAnnotations(const TMap<FString, FString>& annotations)
{
    EPoolGameType game_type;
    LOAD_INT_RETURN("game_type", game_type, EPoolGameType, 
        [](int value) {
            return value >= static_cast<int>(EPoolGameType::GAME_1v1) &&
                   value < static_cast<int>(EPoolGameType::LAST_GAME_TYPE);
        });

    switch (game_type)
    {
    case EPoolGameType::GAME_1v1:
        backend_data = NewObject<UBackend1v1Info>(this);
        break;
    case EPoolGameType::FRIENDLY_GAME_1v1:
        backend_data = NewObject<UBackendFriendly1v1Info>(this);
        break;
    case EPoolGameType::GAME_CONTEST:
        backend_data = NewObject<UBackendContestInfo>(this);
        break;
    case EPoolGameType::TOURN_GAME_1v1:
        backend_data = NewObject<UBackendTourn1v1Info>(this);
        break;
    case EPoolGameType::TOURNAMENT:
        backend_data = NewObject<UBackendTournamentInfo>(this);
        break;
    }

    if (!backend_data->InitStruct(annotations))
    {
        UE_LOG(LogTemp, Error, TEXT("InitStruct wasn't succesful."));
        return false;
    }
    /*auto bd = Cast<UBackend1v1Info>(backend_data);
    UE_LOG(LogTemp, Error, TEXT("User data:"));
    for (auto user : bd->player_info) {
        UE_LOG(LogTemp, Error, TEXT("User_id: %s\nUsername: %s"), *user.user_id, *user.username);
    }*/
    FString transition_url = FString::Format(TEXT("/Game/Maps/{0}?game={1}"), { backend_data->map, backend_data->game_mode });
    UE_LOG(LogTemp, Error, TEXT("InitStruct was succesful. Going to %s"), *transition_url);
    GetWorld()->ServerTravel(transition_url, true, true);

    return true;
}

#undef LOAD_INT_ACTION_IF_FAIL
#undef LOAD_STRING_ACTION_IF_FAIL
#undef PRINT_AND_RETURN
#undef ONLY_PRINT
#undef LOAD_INT
#undef LOAD_STRING
#undef LOAD_INT_RETURN
#undef LOAD_STRING_RETURN

void UPoolServerGameInstance::MakeRequest(FString service,
                                          FString endpoint,
                                          cool_json_t json,
                                          DefaultResponseCallback callback)
{
    MakeRequestWithFail(service, endpoint, json, callback, nullptr);
}


//void UPoolServerGameInstance::KubernetesGetRequestWithFail(FString endpoint,
//                                          DefaultResponseCallback callback,
//                                          DefaultFailedCallback fail_callback)
//{
//    auto func = [this,
//                 callback,
//                 fail_callback,
//                 endpoint](FHttpRequestPtr request,
//                        FHttpResponsePtr response,
//                        bool success) -> void {
//        if (!success)
//        {
//            PRINTF((TEXT("Problem with kubernetes. Request /%s was unsuccesful"), *endpoint), Red);
//            if (fail_callback)
//                fail_callback();
//        }
//        else
//        {
//            cool_json_t json_resp(response);
//            if (!json_resp.is_null())
//            {
//                callback(json_resp);
//            }
//            else
//            {
//                PRINTF((TEXT("Kubernetes Get request: Problem with reading json. Received json is: %s"), *response->GetContentAsString()), Red);
//                if (fail_callback)
//                    fail_callback();
//            }
//        }
//    };
//        
//    
//    FString route = "http://localhost:8001/" + endpoint;
//    
//    
//    auto Request = GetRequest(route);
//    Request->OnProcessRequestComplete().BindLambda(func);
//    Send(Request);
//}
//
//void UPoolServerGameInstance::KubernetesPostRequestWithFail(FString endpoint,
//                                          cool_json_t json,
//                                          DefaultResponseCallback callback,
//                                          DefaultFailedCallback fail_callback)
//{
//    auto func = [this,
//                 callback,
//                 fail_callback,
//                 endpoint](FHttpRequestPtr request,
//                        FHttpResponsePtr response,
//                        bool success) -> void {
//        if (!success)
//        {
//            PRINTF((TEXT("Problem with kubernetes. Request /%s was unsuccesful"), *endpoint), Red);
//            if (fail_callback)
//                fail_callback();
//        }
//        else
//        {
//            cool_json_t json_resp(response);
//            if (!json_resp.is_null())
//            {
//                callback(json_resp);
//            }
//            else
//            {
//                PRINTF((TEXT("Kubernetes Get request: Problem with reading json. Received json is: %s"), *response->GetContentAsString()), Red);
//                if (fail_callback)
//                    fail_callback();
//            }
//        }
//    };
//        
//    
//    FString route = "http://localhost:8001/" + endpoint;
//    
//    
//    auto Request = PostRequest(route, json);
//    Request->OnProcessRequestComplete().BindLambda(func);
//    Send(Request);
//}


void UPoolServerGameInstance::MakeRequestWithFail(FString service,
                                                  FString endpoint,
                                                  cool_json_t json,
                                                  DefaultResponseCallback callback,
                                                  DefaultFailedCallback fail_callback)
{
    auto func = [this,
                 json,
                 callback,
                 fail_callback,
                 service,
                 endpoint](FHttpRequestPtr request,
                        FHttpResponsePtr response,
                        bool success) -> void {
        if (!success)
        {
            PRINTF((TEXT("Problem with server. Request %s/%s was unsuccesful"), *service, *endpoint), Red);
            if (fail_callback)
                fail_callback();
        }
        else
        {
            cool_json_t json_resp(response);
            if (response->GetResponseCode() == 200 && !json_resp.is_null())
            {
                if (callback)
                    callback(json_resp);
            }
            else
            {
                PRINTF((TEXT("Problem with reading json. Recieved json is: %s"), *response->GetContentAsString()), Red);
                if (fail_callback)
                    fail_callback();
            }
        }
    };
    
    FString port;
#define ef else if
    port = "8035";
#undef ef
        
    
    FString route = "http://" + service + ":" + port + "/" + endpoint;
    
    
    auto RequestWithAccessToken = PostRequest(route, json);
    RequestWithAccessToken->OnProcessRequestComplete().BindLambda(func);
    Send(RequestWithAccessToken);
}
