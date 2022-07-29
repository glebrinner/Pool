#pragma once

#include <memory>

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UObject/NoExportTypes.h"
#include "SefaPool/Shared/CueComponent.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "SefaPool/Shared/PoolPlayer.h"
#include "SefaPool/Shared/HTTP_GI.h"
#include "AgonesComponent.h" 
#include "PoolServerGameInstance.generated.h"



UCLASS()
class UBaseBackendInfo : public UObject {
    GENERATED_BODY()
public:
    UPROPERTY()
    FString map;
    UPROPERTY()
    FString game_mode;

    virtual bool InitStruct(const TMap<FString, FString>& annotations);
    virtual FBackendPlayerInfo* GetPlayerInfo(FString user_id);

    virtual EPoolGameType GetGameType();
    virtual cool_json_t GetInfoBundleJson();
};

USTRUCT()
struct FBackendPlayerInfo {
    GENERATED_USTRUCT_BODY()
public:
    UPROPERTY()
    FString user_id;
    UPROPERTY()
    FString username;
    UPROPERTY()
    int cur_cue;
    UPROPERTY()
    int cur_chalk;
    UPROPERTY()
    FItemDurability durability;
    UPROPERTY()
    int shotsPlayed;
    UPROPERTY()
    float totalReactionTime;

};
UCLASS()
class UBackendContestInfo : public UBaseBackendInfo {
    GENERATED_BODY()
public:
    UPROPERTY()
    int contest_id;
    UPROPERTY()
    EDifficultyLevels division;
    UPROPERTY()
    EContestType type_id;
    UPROPERTY()
    int fee;
    UPROPERTY()
    FBackendPlayerInfo player_info;
public:
    virtual bool InitStruct(const TMap<FString, FString>& annotations);
    virtual FBackendPlayerInfo* GetPlayerInfo(FString user_id) override;

    virtual EPoolGameType GetGameType() override;
    virtual cool_json_t GetInfoBundleJson() override;
};

UCLASS()
class UBaseBackend1v1Info : public UBaseBackendInfo {
    GENERATED_BODY()
public:
    UPROPERTY()
    int match_id;
    UPROPERTY()
    EDifficultyLevels difficulty;
    UPROPERTY()
    FBackendPlayerInfo player_info[2];
public:
    virtual bool InitStruct(const TMap<FString, FString>& annotations);
    virtual FBackendPlayerInfo* GetPlayerInfo(FString user_id) override;

    virtual EPoolGameType GetGameType() override;
    virtual cool_json_t GetInfoBundleJson() override;
};

UCLASS()
class UBackend1v1Info : public UBaseBackend1v1Info {
    GENERATED_BODY()
public:
    UPROPERTY()
    int amount;
public:
    virtual bool InitStruct(const TMap<FString, FString>& annotations);

    virtual EPoolGameType GetGameType() override;
    virtual cool_json_t GetInfoBundleJson() override;
};

UCLASS()
class UBackendFriendly1v1Info : public UBackend1v1Info {
    GENERATED_BODY()
public:
    virtual bool InitStruct(const TMap<FString, FString>& annotations);

    virtual EPoolGameType GetGameType() override;
    virtual cool_json_t GetInfoBundleJson() override;
};

UCLASS()
class UBackendTourn1v1Info : public UBaseBackend1v1Info {
    GENERATED_BODY()
public:
    UPROPERTY()
    int tourn_id;
    UPROPERTY()
    int node_id;
    UPROPERTY()
    FString tourn_lobby_external_ip;
    UPROPERTY()
    int tourn_lobby_external_port;

    UPROPERTY()
    FString tournament_pod_ip;
public:
    virtual bool InitStruct(const TMap<FString, FString>& annotations) override;

    virtual EPoolGameType GetGameType() override;
    virtual cool_json_t GetInfoBundleJson() override;
};

UCLASS()
class UBackendTournamentInfo : public UBaseBackendInfo {
    GENERATED_BODY()
public:
    UPROPERTY()
    int tourn_id;
    UPROPERTY()
    int size;
    UPROPERTY()
    EDifficultyLevels difficulty;
public:
    virtual bool InitStruct(const TMap<FString, FString>& annotations);

    virtual EPoolGameType GetGameType() override;
};


const int VERIFICATION_KEY_SIZE = 65;


UCLASS()
class SEFAPOOL_API UPoolServerGameInstance : public UHTTP_GI
{
	GENERATED_BODY()

public:
    
    bool ParseAnnotations(const TMap<FString, FString>& annotations);

    virtual void Init() override;
    
    uint8 tcp_received_key[VERIFICATION_KEY_SIZE];
    
    //void KubernetesGetRequestWithFail(FString endpoint,
    //                                  DefaultResponseCallback callback,
    //                                  DefaultFailedCallback fail_callback);
    //
    //void KubernetesPostRequestWithFail(FString endpoint,
    //                                  cool_json_t json,
    //                                  DefaultResponseCallback callback,
    //                                  DefaultFailedCallback fail_callback);
    
    void MakeRequest(FString service,
                     FString endpoint,
                     cool_json_t json,
                     DefaultResponseCallback callback);

    void MakeRequestWithFail(FString service,
                             FString endpoint,
                             cool_json_t json,
                             DefaultResponseCallback callback,
                             DefaultFailedCallback fail_callback);
public:
    UPROPERTY()
    UBaseBackendInfo* backend_data = nullptr;
};
