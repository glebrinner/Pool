// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolClientGameInstance.h"

void from_json(const cool_json_t& json, FBackendInfo& struct_)
{
    READ_FROM_JSON_STRING(access_token)
    READ_FROM_JSON_STRING(renew_token)
    READ_FROM_JSON_STRING(user_id)
}

FString UPoolClientGameInstance::GetSaveGamePrefix()
{
    if (backend_data.user_id.Len() > 0)
    {
        return backend_data.user_id + TEXT("_");
    }
    return TEXT("default_");
}


void UPoolClientGameInstance::Init()
{
    Super::Init();

    connection_state = EConnectivityState::UNKNOWN;
    Load();
}

void UPoolClientGameInstance::Save()
{
    if (UAuthSaveGame* SaveGameInstance = Cast<UAuthSaveGame>(UGameplayStatics::CreateSaveGameObject(UAuthSaveGame::StaticClass())))
    {
        // Set data on the savegame object.
        SaveGameInstance->renew_token = backend_data.renew_token;
        SaveGameInstance->user_id = backend_data.user_id;

        // Save the data immediately.
        if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, "default_auth", 0))
        {
            PRINTF((TEXT("Auth data saved")), Green)
        }
        else
            PRINTF((TEXT("Auth data not saved")), Red)
    }
    else
        PRINTF((TEXT("Auth data not saved")), Red)
}

void UPoolClientGameInstance::Load()
{
    UAuthSaveGame* LoadedGame = Cast<UAuthSaveGame>(UGameplayStatics::LoadGameFromSlot("default_auth", 0));
    if (LoadedGame)
    {
        PRINTF((TEXT("Auth data loaded")), Green)
        backend_data.renew_token = LoadedGame->renew_token;
        backend_data.user_id = LoadedGame->user_id;
        Renew();
        connection_state = EConnectivityState::ONLINE;
    }
    else
        PRINTF((TEXT("Auth data not loaded")), Green)
}


void UPoolClientGameInstance::MakeRequestWithInitToken(FString service,
                                                       FString endpoint,
                                                       cool_json_t cool_json,
                                                       FullResponceCallback callback)
{
    auto InitTokenRequest = GetRequest(ApiBaseUrl + "request_token");
    InitTokenRequest->OnProcessRequestComplete().BindLambda(
        [=](FHttpRequestPtr request, FHttpResponsePtr response, bool success) -> void {
        if (!success)
        {
            PRINTF((TEXT("Failed to get init token for %s/%s"), *service, *service), Red);
        }
        else
        {
            cool_json_t request_token_response = cool_json_t::parse(TCHAR_TO_UTF8(*response->GetContentAsString()), nullptr, false);
            if (request_token_response.is_discarded())
                request_token_response = cool_json_t();
            cool_json_t req_json = cool_json;
            if (request_token_response.contains("init_token"))
                req_json["init_token"] = request_token_response["init_token"];
            FString route = ApiBaseUrl + service + "/" + endpoint;
            auto Request = PostRequest(route, req_json);
            Request->SetHeader(TEXT("init-token"), request_token_response["init_token"].get<FString>());
            Request->OnProcessRequestComplete().BindLambda(callback);
            Send(Request);
        }
    });
    Send(InitTokenRequest);
}

void UPoolClientGameInstance::MakeRequestWithAccessToken(FString service,
                                                         FString endpoint,
                                                         cool_json_t json,
                                                         FullResponceCallback callback)
{
    json["access_token"] = backend_data.access_token;
    json["user_id"] = backend_data.user_id;
    FString route = ApiBaseUrl + service + "/" + endpoint;
    auto RequestWithAccessToken = PostRequest(route, json);
    RequestWithAccessToken->SetHeader(TEXT("access-token"), backend_data.access_token);
    RequestWithAccessToken->SetHeader(TEXT("user-id"), backend_data.user_id);
    RequestWithAccessToken->OnProcessRequestComplete().BindLambda(callback);
    Send(RequestWithAccessToken);
}

void UPoolClientGameInstance::MakeRequestWithAccessTokenAndProgress(FString service,
                                                         FString endpoint,
                                                         cool_json_t json,
                                                         FullResponceCallback callback,
                                                         FullProgressCallback prog_callback)
{
    json["access_token"] = backend_data.access_token;
    json["user_id"] = backend_data.user_id;
    FString route = ApiBaseUrl + service + "/" + endpoint;
    auto RequestWithAccessToken = PostRequest(route, json);
    RequestWithAccessToken->SetHeader(TEXT("access-token"), backend_data.access_token);
    RequestWithAccessToken->SetHeader(TEXT("user-id"), backend_data.user_id);
    RequestWithAccessToken->OnProcessRequestComplete().BindLambda(callback);
    RequestWithAccessToken->OnRequestProgress().BindLambda(prog_callback);
    Send(RequestWithAccessToken);
}

void UPoolClientGameInstance::Login(const FString &username,
                                    const FString& password,
                                    const FResponseDelegat& succesful,
                                    const FResponseDelegat& unsuccesful)
{
    cool_json_t json;
    json["username"] = username;
    json["password"] = password;

    // Callback when login request is completed
    auto func = [this, succesful, unsuccesful](FHttpRequestPtr request, FHttpResponsePtr response, bool success) -> void {
        if (!success)
        {
            PRINTF((TEXT("Problem with server. Request %s was unsuccesful"), TEXT("RequestLogin")), Red);
            UE_LOG(LogTemp, Error, TEXT("Problem with server. Request %s was unsuccesful"), TEXT("RequestLogin"))
            unsuccesful.Execute();
        }
        else
        {
            cool_json_t json_resp(response);
            UE_LOG(LogTemp, Warning, TEXT("Login response %s"), *FString(json_resp.dump(2).c_str()))
            
            if (!json_resp.is_null())
            {
                if (true || check_status(json_resp, "auth/login"))
                {
                    if (!json_resp["match"].get<bool>())
                    {
                        unsuccesful.Execute();
                    }
                    else
                    {
                        json_resp.get_to(backend_data);
                        connection_state = EConnectivityState::ONLINE;
                        succesful.Execute();
                        Save();
                    }
                }
                else
                {
                    unsuccesful.Execute();
                }
            }
            else
            {
                PRINTF((TEXT("Problem with reading json. Recieved json is: %s"), *response->GetContentAsString()), Red);
                unsuccesful.Execute();
            }
        }
    };

    MakeRequestWithInitToken("auth", "login", json, func);
}

void UPoolClientGameInstance::PlayingOffline()
{
    connection_state = EConnectivityState::OFFLINE;
}

void UPoolClientGameInstance::Renew()
{
    cool_json_t json;
    json["renew_token"] = backend_data.renew_token;
    json["user_id"] = backend_data.user_id;
    // Callback when login request is completed
    auto func = [this](FHttpRequestPtr request,
                       FHttpResponsePtr response,
                       bool success) -> void {
        if (!success)
        {
            PRINTF((TEXT("Problem with server. Request %s was unsuccesful"), TEXT("Renew")), Red);
        }
        else
        {
            cool_json_t json_resp(response);
            
            if (!json_resp.is_null())
            {
                if (check_status(json_resp, "auth/renew"))
                {
                    if (!json_resp["match"].get<bool>())
                    {
                        PRINTF((TEXT("Didn't find given pair login, password. Login or password are incorrect.")), Red);
                    }
                    else
                    {
                        json_resp.get_to(backend_data);
                        Save();
                    }
                }
            }
            else
            {
                PRINTF((TEXT("Problem with reading json. Received json is: %s"), *response->GetContentAsString()), Red);
            }
        }
    };

    MakeRequestWithInitToken("auth", "renew", json, func);
}

void UPoolClientGameInstance::Logout()
{
    cool_json_t json;
    json["renew_token"] = backend_data.renew_token;

    MakeRequest("auth", "logout", json, [this](cool_json_t){
        UE_LOG(LogTemp, Warning, TEXT("Logged out"))
        backend_data.access_token.Empty();
        backend_data.renew_token.Empty();
        backend_data.user_id.Empty();
    });
}

void UPoolClientGameInstance::FullLogout()
{
    MakeRequest("auth", "force_logout", cool_json_t(), [this](cool_json_t){
        UE_LOG(LogTemp, Warning, TEXT("Full Logged out"))
        backend_data.access_token.Empty();
        backend_data.renew_token.Empty();
        backend_data.user_id.Empty();
    });
}

void UPoolClientGameInstance::MakeRequest(FString service,
                                          FString endpoint,
                                          cool_json_t json,
                                          DefaultResponseCallback callback,
                                          bool renew)
{
    MakeRequestWithFail(service, endpoint, json, callback, nullptr);
}

void UPoolClientGameInstance::MakeRequestWithFail(FString service,
                                                  FString endpoint,
                                                  cool_json_t json,
                                                  DefaultResponseCallback callback,
                                                  DefaultFailedCallback fail_callback,
                                                  bool renew)
{
    
    PRINTF((TEXT("Make request: %s%s/%s"), *ApiBaseUrl, *service, *endpoint), Red);
    UE_LOG(LogTemp, Warning, TEXT("Make request: %s%s/%s\nContent:\n%s\n"), *ApiBaseUrl, *service, *endpoint, *FString(json.dump(2).c_str()));
    
    if (backend_data.access_token.Len() < 64)
    {
        if (fail_callback)
            fail_callback();
        UE_LOG(LogTemp, Warning, TEXT("Aborting request %s%s/%s: no access token\n"), *ApiBaseUrl, *service, *endpoint);
        return;
    }
    
    auto func = [this,
                 json,
                 callback,
                 fail_callback,
                 renew,
                 service,
                 endpoint](FHttpRequestPtr request,
                        FHttpResponsePtr response,
                        bool success) -> void {
        if (!success)
        {
            PRINTF((TEXT("Problem with server. Request %s/%s was unsuccesful"), *service, *endpoint), Red);
            if (fail_callback != nullptr)
                fail_callback();
        }
        else
        {
            cool_json_t json_resp(response);
            if (response->GetResponseCode() == 401 && renew)
            {
                PRINTF((TEXT("%s/%s: Returned 401, renewing"), *service, *endpoint), Red)
                Renew();
                MakeRequestWithFail(service, endpoint, json, callback, fail_callback, false);
            }
            else if (response->GetResponseCode() == 200 && !json_resp.is_null())
            {
                if (callback)
                    callback(json_resp);
            }
            else
            {
                PRINTF((TEXT("%s/%s: Problem with reading json. Response code: %d. Received json is: %s"), *service, *endpoint, response->GetResponseCode(), *response->GetContentAsString()), Red);
                if (fail_callback)
                    fail_callback();
            }
        }
    };
    MakeRequestWithAccessToken(service, endpoint, json, func);
}

void UPoolClientGameInstance::MakeRequestWithFailAndProgress(FString service,
                                                             FString endpoint,
                                                             cool_json_t json,
                                                             DefaultResponseCallback callback,
                                                             DefaultFailedCallback fail_callback,
                                                             DefaultProgressCallback progress_callback,
                                                             bool renew)
{
    
    PRINTF((TEXT("Make request: %s%s/%s"), *ApiBaseUrl, *service, *endpoint), Red);
    UE_LOG(LogTemp, Warning, TEXT("Make request: %s%s/%s\nContent:\n%s\n"), *ApiBaseUrl, *service, *endpoint, *FString(json.dump(2).c_str()));
    
    if (backend_data.access_token.Len() < 64)
    {
        if (fail_callback)
            fail_callback();
        UE_LOG(LogTemp, Warning, TEXT("Aborting request %s%s/%s: no access token\n"), *ApiBaseUrl, *service, *endpoint);
        return;
    }
    
    auto progress_func = [progress_callback](FHttpRequestPtr HttpRequest,
                            int32 BytesSend,
                            int32 InBytesReceived) {
        FHttpResponsePtr HttpResponse = HttpRequest->GetResponse();
        if (HttpResponse.IsValid())
        {
            int32 TotalSize = HttpResponse->GetContentLength(); // Correct, this is the size of the payload RECEIVED in the RESPONSE
            float PercentDone = ((float)InBytesReceived / (float)TotalSize);
            progress_callback(PercentDone);
        }
    };
    
    auto func = [this,
                 json,
                 callback,
                 fail_callback,
                 progress_callback,
                 renew,
                 service,
                 endpoint](FHttpRequestPtr request,
                        FHttpResponsePtr response,
                        bool success) -> void {
        if (!success)
        {
            PRINTF((TEXT("Problem with server. Request %s/%s was unsuccesful"), *service, *endpoint), Red);
            if (fail_callback != nullptr)
                fail_callback();
        }
        else
        {
            cool_json_t json_resp(response);
            if (response->GetResponseCode() == 401 && renew)
            {
                PRINTF((TEXT("%s/%s: Returned 401, renewing"), *service, *endpoint), Red)
                Renew();
                MakeRequestWithFailAndProgress(service, endpoint, json, callback, fail_callback, progress_callback, false);
            }
            else if (response->GetResponseCode() == 200 && !json_resp.is_null())
            {
                if (callback)
                    callback(json_resp);
            }
            else
            {
                PRINTF((TEXT("%s/%s: Problem with reading json. Response code: %d. Received json is: %s"), *service, *endpoint, response->GetResponseCode(), *response->GetContentAsString()), Red);
                if (fail_callback)
                    fail_callback();
            }
        }
    };
    MakeRequestWithAccessTokenAndProgress(service, endpoint, json, func, progress_func);
}


void UPoolClientGameInstance::CheckConnectivity(const FResponseDelegat& succesful, const FResponseDelegat& unsuccesful)
{
    PRINTF((TEXT("Checking connectivity")), Red);
    PRINTF((TEXT("Make request: %s%s"), *ApiBaseUrl, TEXT("healthcheck")), Red);
    
    FString route = ApiBaseUrl + "healthcheck";
    auto Request = GetRequest(route);
    
    auto callback = [=](FHttpRequestPtr request, FHttpResponsePtr response, bool success) -> void {
        if (!success || (response && response->GetResponseCode() != 200))
        {
            connection_state = EConnectivityState::NO_CONNECTIVITY;
            unsuccesful.Execute();
        }
        else
        {
            succesful.Execute();
            if (connection_state == EConnectivityState::NO_CONNECTIVITY || 
                connection_state == EConnectivityState::OFFLINE)
            connection_state = EConnectivityState::ONLINE_BUT_NOT_LOGGED_IN;
        }
    };
    
    Request->OnProcessRequestComplete().BindLambda(callback);
    Send(Request);
}
