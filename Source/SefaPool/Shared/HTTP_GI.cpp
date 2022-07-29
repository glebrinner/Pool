
#include "HTTP_GI.h"

#include "GameFramework/Actor.h"
#include "Runtime/Online/HTTP/Public/Http.h"
#include "Json.h"
#include "../Shared/SharedDefs.h"
#include "Runtime/JsonUtilities/Public/JsonObjectConverter.h"
#include "../SefaPool.h"

bool UHTTP_GI::InitIfNeeded()
{
    if (Http) return true;
    Http = &FHttpModule::Get();
    Http->SetHttpTimeout(2.0f);
    if (!Http) return false;
    return true;
}

/**********************************************************************************************************************************************/


TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UHTTP_GI::RequestWithRoute(FString Route) {
    InitIfNeeded();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();
    Request->SetURL(Route);
    SetRequestHeaders(Request);
    return Request;
}

void UHTTP_GI::SetRequestHeaders(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request) {
    InitIfNeeded();
    //Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    //Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UHTTP_GI::GetRequest(FString Subroute) {
    InitIfNeeded();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = RequestWithRoute(Subroute);
    Request->SetVerb("GET");
    return Request;
}

TSharedRef<IHttpRequest, ESPMode::ThreadSafe> UHTTP_GI::PostRequest(FString Subroute, cool_json_t cool_json) {
    InitIfNeeded();
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = RequestWithRoute(Subroute);
    Request->SetVerb("POST");
    Request->SetContentAsString(FString(cool_json.dump().c_str()));
    return Request;
}

void UHTTP_GI::Send(TSharedRef<IHttpRequest, ESPMode::ThreadSafe>& Request) {
    InitIfNeeded();
    Request->ProcessRequest();
}

TSharedPtr<FJsonObject> CoolJsonToLameJson(cool_json_t json)
{
    FString json_str(json.dump().c_str());
    TSharedPtr<FJsonObject> lame_json;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(json_str);
    if (FJsonSerializer::Deserialize(Reader, lame_json))
    {
        return lame_json;
    }
    TSharedPtr<FJsonObject> empty = MakeShareable(new FJsonObject);
    return empty;
}

cool_json_t LameJsonToCoolJson(TSharedPtr<FJsonObject> json)
{
    FString OutputString;
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(json.ToSharedRef(), Writer);
    std::string json_string(TCHAR_TO_UTF8(*OutputString));
    cool_json_t cool_json = cool_json_t::parse(json_string, nullptr, false);
    if (cool_json.is_discarded())
        cool_json = cool_json_t();
    return cool_json;
}



/*
template <typename Func>
void UHTTP_GI::MakeRequestWithAccessToken(FString address, cool_json_t cool_json, Func callback)
{
    TSharedPtr<FJsonObject> json = CoolJsonToLameJson(cool_json);
    
    USefaPoolGameInstance* GameInstance = Cast<USefaPoolGameInstance>(GetGameInstance());
    json->SetStringField("access_token", GameInstance->entry_data.backend_info.access_token);
    json->SetStringField("user_id", GameInstance->entry_data.backend_info.user_id);
    auto RequestWithAccessToken = PostRequest(address, json);
    RequestWithAccessToken->OnProcessRequestComplete().BindLambda(callback);
    Send(RequestWithAccessToken);
}
*/
void UHTTP_GI::MakeRepeatRequest(FString address,
                                 cool_json_t json,
                                 DefaultResponseCallback succesful_callback,
                                 DefaultFailedCallback unsuccesful_callback,
                                 int max_attempt_count)
{
    InitIfNeeded();
    auto RepeatRequest = PostRequest(address, json);
    int *current_attempt = new int(0);

    std::function<void(FHttpRequestPtr, FHttpResponsePtr, bool)>** repeat_lambda_ptr = new std::function<void(FHttpRequestPtr, FHttpResponsePtr, bool)>*;
    *repeat_lambda_ptr = new std::function<void(FHttpRequestPtr, FHttpResponsePtr, bool)> (
        [this, address, json, succesful_callback, unsuccesful_callback, current_attempt, max_attempt_count, repeat_lambda_ptr](FHttpRequestPtr request, FHttpResponsePtr response, bool success) -> void {
        if (!success || response->GetResponseCode() != 200)
        {
            *current_attempt += 1;
            if (*current_attempt < max_attempt_count)
            {
                auto RepeatRequest = PostRequest(address, json);
                RepeatRequest->OnProcessRequestComplete().BindLambda(**repeat_lambda_ptr);
                Send(RepeatRequest);
            }
            else
            {
                delete current_attempt;
                unsuccesful_callback();
                delete* repeat_lambda_ptr;
                delete repeat_lambda_ptr;
            }
        }
        else
        {
            succesful_callback(cool_json_t(response));
            delete current_attempt;
            delete *repeat_lambda_ptr;
            delete repeat_lambda_ptr;
        }
    });

    RepeatRequest->OnProcessRequestComplete().BindLambda(**repeat_lambda_ptr);
    Send(RepeatRequest);
    
    //succesful_callback();
}

bool UHTTP_GI::ResponseIsValid(FHttpResponsePtr Response, bool bWasSuccessful) {
    InitIfNeeded();
    if (!bWasSuccessful || !Response.IsValid()) return false;
    if (EHttpResponseCodes::IsOk(Response->GetResponseCode())) return true;
    else {
        UE_LOG(LogTemp, Warning, TEXT("Http Response returned error code: %d"), Response->GetResponseCode());
        return false;
    }
}

/**********************************************************************************************************************************************/


/*
template <typename StructType>
void UHTTP_GI::GetJsonStringFromStruct(StructType FilledStruct, FString& StringOutput) {
    FJsonObjectConverter::UStructToJsonObjectString(StructType::StaticStruct(), &FilledStruct, StringOutput, 0, 0);
}

template <typename StructType>
void UHTTP_GI::GetStructFromJsonString(FHttpResponsePtr Response, StructType& StructOutput) {
    StructType StructData;
    FString JsonString = Response->GetContentAsString();
    FJsonObjectConverter::JsonObjectStringToUStruct<StructType>(JsonString, &StructOutput, 0, 0);
}
*/
// Check field "status" in given json.
// If status wasn't 200 print service and status to LOG and return false. Otherwise return true.
bool check_status(cool_json_t json, FString service)
{
    if (json.is_null() ||
        !json.contains("status") ||
        !json["status"].is_number())
        return false;
    int status = json["status"].get<int>();
    if (status != 200)
    {
        PRINTF((TEXT("Service %s returned status %d"), *service, status), Red);
        UE_LOG(LogTemp, Warning, TEXT("Service %s returned status %d"), *service, status);
    }
    return status == 200;
};

void to_json(cool_json_t& json, const FString& str)
{
    std::string json_string(TCHAR_TO_UTF8(*str));
    cool_json_t cool_json = cool_json_t(json_string);
    json = cool_json;
}

void to_json(cool_json_t& json, const FHttpResponsePtr & response)
{
    std::string json_string(TCHAR_TO_UTF8(*response->GetContentAsString()));
    cool_json_t cool_json = cool_json_t::parse(json_string, nullptr, false);
    if (!cool_json.is_discarded())
        json = cool_json;
    else
        json = cool_json_t();
}

void from_json(const cool_json_t& json, FString& str)
{
    if (json.is_string())
    {
        str = FString(json.get<std::string>().c_str());
    }
}

#define ADD_FUNC(type) \
bool UHTTP_GI::Add_##type(FCoolJsonBP& jsonBP, const FString& str, type value)\
{\
    std::string key(TCHAR_TO_UTF8(*str));\
    if (jsonBP.json.contains(key)) return false;\
    jsonBP.json[key] = value;\
    return true;\
}

ADD_FUNC(int)
ADD_FUNC(FString)
ADD_FUNC(float)

#undef ADD_FUNC

#define GET_FUNC(type, check_name) \
type UHTTP_GI::Get_##type(FCoolJsonBP& jsonBP, const FString& str)\
{\
    std::string key(TCHAR_TO_UTF8(*str));\
    if (jsonBP.json.contains(key)) return type();\
    if (!jsonBP.json.is_##check_name()) return type();\
    type var;\
    jsonBP.json[key].get_to(var);\
    return var;\
}

GET_FUNC(int, number)
GET_FUNC(FString, string)
GET_FUNC(float, number)

#undef GET_FUNC

bool UHTTP_GI::Contains(FCoolJsonBP& jsonBP, const FString& str)
{
    std::string key(TCHAR_TO_UTF8(*str));
    return jsonBP.json.contains(key);
}

#define IS_FUNC(type, check_name) \
bool UHTTP_GI::Is_##type(FCoolJsonBP& jsonBP, const FString& str)\
{\
    std::string key(TCHAR_TO_UTF8(*str));\
    if (jsonBP.json.contains(key)) return false;\
    return jsonBP.json.is_##check_name();\
}

IS_FUNC(int, number)
IS_FUNC(FString, string)
IS_FUNC(float, number)

#undef IS_FUNC

