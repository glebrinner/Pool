#pragma once

//#define BUILD_FOR_MOBILE

#include "Engine.h"

#ifndef __FUNCSIG__
    #ifdef __PRETTY_FUNCTION__
        #define __FUNCSIG__ __PRETTY_FUNCTION__
    #else
        #define __FUNCSIG__ ""
    #endif
#else
    #if !defined(__PRETTY_FUNCTION__)
    #define __PRETTY_FUNCTION__ __FUNCSIG__
    #endif
#endif

#define LOG_FUNC UE_LOG(LogTemp, Warning, TEXT("LOGGING__ Starting: %s"), *FString(__PRETTY_FUNCTION__));


#if (PLATFORM_ANDROID || PLATFORM_IOS) && UE_BUILD_SHIPPING

#define API_URL "https://eks.sefapool-test.gotbitgames.co/"
#define LOBBY_URL "NO"
#define GS_IP_FOR_CLIENTS "NO"

#elif (PLATFORM_ANDROID || PLATFORM_IOS) && !UE_BUILD_SHIPPING

#define API_URL "https://eks.sefapool-test.gotbitgames.co/"
#define LOBBY_URL "NO"
#define GS_IP_FOR_CLIENTS "NO"

#else // not for mobile, any target

#define API_URL "https://eks.sefapool-test.gotbitgames.co/"
#define LOBBY_URL "NO"
#define GS_IP_FOR_CLIENTS "NO"

#endif

#define NETMODE_WORLD (((GEngine == nullptr) || (GetWorld() == nullptr)) ? TEXT("") \
        : (GEngine->GetNetMode(GetWorld()) == NM_Client) ? TEXT("[Client] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_ListenServer) ? TEXT("[ListenServer] ") \
        : (GEngine->GetNetMode(GetWorld()) == NM_DedicatedServer) ? TEXT("[DedicatedServer] ") \
        : TEXT("[Standalone] "))

#ifdef _MSC_VER
#define FUNC_NAME    TEXT(__PRETTY_FUNCTION__)
#else // FIXME - GCC?
#define FUNC_NAME    TEXT(__FUNCSIG__)
#endif

#define TRACE(Format, ...) \
{ \
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    if (Msg == "") \
    { \
        UE_LOG(LogTemp, Log, TEXT("\u001b[36m%s%s() : %s\u001b[0m"), NETMODE_WORLD, FUNC_NAME, *GetNameSafe(this));\
    } \
    else \
    { \
        UE_LOG(LogTemp, Log, TEXT("\u001b[36m%s%s() : %s\u001b[0m"), NETMODE_WORLD, FUNC_NAME, *Msg);\
    } \
}

#define TRACESTATIC(Format, ...) \
{ \
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(LogTemp, Log, TEXT("%s() : %s"), FUNC_NAME, *Msg);\
}

#define TRACEWARN(Format, ...) \
{ \
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(LogTemp, Warning, TEXT("**WARNING** %s%s() : %s"), NETMODE_WORLD, FUNC_NAME, *Msg);\
}

#define TRACEERROR(Format, ...) \
{ \
    SET_WARN_COLOR( COLOR_RED );\
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    UE_LOG(LogTemp, Error, TEXT("**ERROR** %s%s() : %s"), NETMODE_WORLD, FUNC_NAME, *Msg);\
    CLEAR_WARN_COLOR();\
}

#define SCREENDEBUG(Format, ...) \
{ \
    const FString Msg = FString::Printf(TEXT(Format), ##__VA_ARGS__); \
    if (Msg == "") \
    { \
        TCHAR StdMsg[MAX_SPRINTF] = TEXT(""); \
        FCString::Sprintf(StdMsg, TEXT("%s%s() : %s"), NETMODE_WORLD, FUNC_NAME, *GetNameSafe(this)); \
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, StdMsg); \
    } \
    else \
    { \
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::White, Msg); \
    } \
}

#ifdef BUILD_FOR_MOBILE
#define PRINTF(s, c) {}

#define ERROR_DEBUG_PRINT_FUNC \
{ \
    return;  \
}
#define ERROR_DEBUG_PRINT_FUNC_RET(ret) \
{ \
    return ret; \
}

#define ERROR_DEBUG_PRINT_FUNC_INFO(info) \
{ \
    return; \
}

#define ERROR_DEBUG_PRINT_FUNC_INFO_RET(info, ret) \
{ \
    return ret; \
}

#else 

#define PRINTF(string, color) \
{if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 150.0f, FColor::color, FString::Printf string); \
}

#define PRINT_BOOL(p) PRINTF((TEXT("%s: %s\n"), TEXT(#p), p ? TEXT("True") : TEXT("False")), Red);

#define ERROR_DEBUG_PRINT_FUNC \
{ \
    UE_LOG(LogTemp, Warning, TEXT("Line: %d func: %s\n error"), __LINE__, __FUNCSIG__); \
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, __FUNCSIG__ " error"); \
    return; \
}
#define ERROR_DEBUG_PRINT_FUNC_RET(ret) \
{ \
    UE_LOG(LogTemp, Warning, TEXT(__FUNCSIG__ " error")); \
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, __FUNCSIG__ " error"); \
    return ret; \
}

#define ERROR_DEBUG_PRINT_FUNC_INFO(info) \
{ \
    UE_LOG(LogTemp, Warning, TEXT(__FUNCSIG__ " error: " info)); \
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, __FUNCSIG__ " error: " info); \
    return; \
}

#define ERROR_DEBUG_PRINT_FUNC_INFO_RET(info, ret) \
{ \
    UE_LOG(LogTemp, Warning, TEXT(__FUNCSIG__ " error: " info)); \
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, __FUNCSIG__ " error: " info); \
    return ret; \
}

#endif

//#define TRUNCATE_FLOAT(x) ((float)((int)(x*1e5f)))*1e-5f
//#define TRUNCATE_VECTOR(v) FVector(TRUNCATE_FLOAT(v.X), TRUNCATE_FLOAT(v.Y), TRUNCATE_FLOAT(v.Z))
#define TRUNCATE_FLOAT(x) x
#define TRUNCATE_VECTOR(v) v

#define READ_FROM_JSON_STRING_(var_name, json_name) \
if (json.contains(json_name) && \
    json[json_name].is_string()) \
json[json_name].get_to(struct_.var_name);

#define READ_FROM_JSON_STRING(var_name) \
READ_FROM_JSON_STRING_(var_name, #var_name)

#define READ_FROM_JSON_NUM_(var_name, json_name) \
if (json.contains(json_name) && \
    json[json_name].is_number()) \
json[json_name].get_to(struct_.var_name);

#define READ_FROM_JSON_NUM(var_name) READ_FROM_JSON_NUM_(var_name, #var_name)


const float GAME_UNIT_IN_CM = 0.8f;
const float FREE_FALL_ACCELERATION = 980.0f / GAME_UNIT_IN_CM;
const float MAX_BALL_LINEAR_VELOCITY = 1200.0f / GAME_UNIT_IN_CM;
const float MIN_SHOT_TIME = 15.0f;
const float MAX_SHOT_TIME = 35.0f;
const float CIRCLE_OF_BALLS_RADIUS = 0.333333f;


const int BALL_COUNT_IN_SCENE = 55;

const size_t ALLOCATION_REQUEST_CONCURRENCY = 2;
