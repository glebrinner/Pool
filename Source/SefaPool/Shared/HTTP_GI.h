// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SharedDefs.h"

#include "Runtime/Online/HTTP/Public/Http.h"


#include "../ThirdParty/json.hpp"
typedef nlohmann::json cool_json_t;
typedef std::function<void(cool_json_t)> DefaultResponseCallback;
typedef std::function<void()> DefaultFailedCallback;


#include "HTTP_GI.generated.h"



USTRUCT(BlueprintType)
struct FCoolJsonBP
{
    GENERATED_USTRUCT_BODY()
public:
    cool_json_t json;
};



DECLARE_DYNAMIC_DELEGATE(FResponseDelegat);


UCLASS()
class SEFAPOOL_API UHTTP_GI : public UGameInstance
{
    GENERATED_BODY()
    
    
    FHttpModule* Http;
    
    bool InitIfNeeded();
    
public:

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> RequestWithRoute(FString Subroute);
    void SetRequestHeaders(TSharedRef<IHttpRequest,
                           ESPMode::ThreadSafe>& Request);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> GetRequest(FString Subroute);
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> PostRequest(FString Subroute,
                                                              cool_json_t cool_json);
    void Send(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request);

    
    void MakeRepeatRequest(FString address,
                           cool_json_t cool_json,
                           DefaultResponseCallback succesful_callback,
                           DefaultFailedCallback unsuccesful_callback, int max_attempt_count = 3);

    bool ResponseIsValid(FHttpResponsePtr Response,
                         bool bWasSuccessful);
    /*
    UFUNCTION(BlueprintCallable)
    FCoolJsonBP CreateJson();*/
    
#define TYPED_FUNCS(type)\
UFUNCTION(BlueprintCallable)\
bool Add_##type(FCoolJsonBP& jsonBP, const FString& str, type value);\
UFUNCTION(BlueprintCallable)\
type Get_##type(FCoolJsonBP& jsonBP, const FString& str);\
UFUNCTION(BlueprintCallable)\
bool Is_##type(FCoolJsonBP& jsonBP, const FString& str);

    UFUNCTION(BlueprintCallable)
    bool Contains(FCoolJsonBP& jsonBP, const FString& str);
    
    TYPED_FUNCS(int)
    TYPED_FUNCS(FString)
    TYPED_FUNCS(float)
#undef TYPED_FUNCS
    
};
bool check_status(cool_json_t cool_json, FString service);
    


void to_json(cool_json_t& json, const FString& str);
void from_json(const cool_json_t& json, FString& str);

void to_json(cool_json_t& json, const FHttpResponsePtr & response);
    
    
TSharedPtr<FJsonObject> CoolJsonToLameJson(cool_json_t json);

cool_json_t LameJsonToCoolJson(TSharedPtr<FJsonObject> json);

#undef TYPED_FUNCS
