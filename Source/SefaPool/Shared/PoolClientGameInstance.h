// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTTP_GI.h"
#include "SharedDefs.h"
#include "PoolClientGameInstance.generated.h"

typedef std::function<void(FHttpRequestPtr request,
                           FHttpResponsePtr response,
                           bool)> FullResponceCallback;

typedef std::function<void(FHttpRequestPtr, int32, int32)> FullProgressCallback;

typedef std::function<void(float)> DefaultProgressCallback;

USTRUCT(BlueprintType)
struct FBackendInfo
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "BackendData")
        FString access_token;
    UPROPERTY(BlueprintReadWrite, Category = "BackendData")
        FString renew_token;
    UPROPERTY(BlueprintReadWrite, Category = "BackendData")
        FString user_id;
};

void from_json(const cool_json_t& json, FBackendInfo& bi);

UCLASS()
class SEFAPOOL_API UAuthSaveGame : public USaveGame
{
    GENERATED_BODY()

    public:
    
    UPROPERTY()
    FString renew_token;
    UPROPERTY()
    FString user_id;
};


UENUM(BlueprintType)
enum class EConnectivityState : uint8
{
    UNKNOWN,
    ONLINE_BUT_NOT_LOGGED_IN,
    ONLINE,
    NO_CONNECTIVITY, // no internet connection/backend is down
    OFFLINE          // conscious decision to play offline
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FJsonResponseBP, const FCoolJsonBP&, response);

UCLASS()
class SEFAPOOL_API UPoolClientGameInstance : public UHTTP_GI
{
    GENERATED_BODY()
    
    FString ApiBaseUrl = API_URL;
    
    void Renew();
    void Save();
    void Load();
    
public:
    void Init();
    FString GetSaveGamePrefix();
	
    void MakeRequestWithInitToken(FString service,
                                  FString endpoint,
                                  cool_json_t cool_json,
                                  FullResponceCallback callback);
    
    void MakeRequestWithAccessToken(FString service,
                                    FString endpoint,
                                    cool_json_t json,
                                    FullResponceCallback callback);
    
    void MakeRequestWithAccessTokenAndProgress(FString service,
                                               FString endpoint,
                                               cool_json_t json,
                                               FullResponceCallback callback,
                                               FullProgressCallback prog_callback);
    
    UPROPERTY(BlueprintReadWrite)
    FBackendInfo backend_data;
    
    void Login(const FString &username,
               const FString& password,
               const FResponseDelegat& succesful,
               const FResponseDelegat& unsuccesful);

    virtual void PlayingOffline();
    
    void Logout();
    void FullLogout();

    void MakeRequest(FString service,
                     FString endpoint,
                     cool_json_t json,
                     DefaultResponseCallback callback,
                     bool renew = true);

    void MakeRequestWithFail(FString service,
                             FString endpoint,
                             cool_json_t json,
                             DefaultResponseCallback callback,
                             DefaultFailedCallback fail_callback,
                             bool renew = true);
    
    void MakeRequestWithFailAndProgress(FString service,
                                        FString endpoint,
                                        cool_json_t json,
                                        DefaultResponseCallback callback,
                                        DefaultFailedCallback fail_callback,
                                        DefaultProgressCallback progress_callback,
                                        bool renew = true);
    
    

    UPROPERTY(BlueprintReadOnly)
    EConnectivityState connection_state;
    
    void CheckConnectivity(const FResponseDelegat& succesful,
                           const FResponseDelegat& unsuccesful);
    
};
