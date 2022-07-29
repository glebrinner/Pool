
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolReplays.generated.h"

UENUM(BlueprintType)
enum class EPoolGameType : uint8
{
    GAME_1v1 = 0 UMETA(DisplayName = "Game 1v1"),
    GAME_CONTEST = 1 UMETA(DisplayName = "Contest"),
    TOURN_GAME_1v1 = 2 UMETA(DisplayName = "Tourn Game 1v1"),
    TOURNAMENT = 3 UMETA(DisplayName = "Tournament"),
    FRIENDLY_GAME_1v1 = 4 UMETA(DisplayName = "Friendly game 1v1"),
    LAST_GAME_TYPE = 5 UMETA(DisplayName = "No type")
};

UENUM(BlueprintType)
enum class EReplayDownloadStatus : uint8
{
    ONLINE = 0      UMETA(DisplayName = "ONLINE"),
    DOWNLOADING = 1 UMETA(DisplayName = "DOWNLOADING"),
    OFFLINE = 2     UMETA(DisplayName = "OFFLINE")
};

enum class EDifficultyLevels : uint8;
enum class EContestType : uint8;

USTRUCT(BlueprintType)
struct FReplayInfo
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(BlueprintReadOnly)
    FString id_;
    UPROPERTY(BlueprintReadOnly)
    FString room;
    UPROPERTY(BlueprintReadOnly)
    FString gm;
    UPROPERTY(BlueprintReadOnly)
    EPoolGameType type;
    UPROPERTY(BlueprintReadOnly)
    EReplayDownloadStatus downloadStatus;
    UPROPERTY(BlueprintReadOnly)
    float downloadProgress;
    UPROPERTY(BlueprintReadOnly)
    bool personal;
    UPROPERTY(BlueprintReadOnly)
    FString opponentId;
    UPROPERTY(BlueprintReadOnly)
    FString opponentUsername;
    UPROPERTY(BlueprintReadOnly)
    bool won;
    UPROPERTY(BlueprintReadOnly)
    int amount;
    UPROPERTY(BlueprintReadOnly)
    int tournament_node_id;
    UPROPERTY(BlueprintReadOnly)
    EDifficultyLevels diffLevel;
    UPROPERTY(BlueprintReadOnly)
    EContestType contestType;

    
    bool presentOnline;
    
    bool operator==(const FReplayInfo& that);
    
};


UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolReplays : public USefaPoolDataBlock
{
    GENERATED_BODY()
    
public:
    
    UPROPERTY(BlueprintReadOnly)
    TArray<FReplayInfo> replays;
    
    FString replayFileToPlay;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    UFUNCTION(BlueprintCallable)
    void DownloadReplay(FString replayId);
    
    void ProcessDownloadedReplay(FString id_, const FString& data);
    UFUNCTION(BlueprintCallable)
    FReplayInfo GetReplayInfoForId(FString id_);
    UFUNCTION(BlueprintCallable)
    bool FillReplayFilename(FString id_);
    UFUNCTION(BlueprintCallable)
    TArray<FReplayInfo> GetReplaysFiltered(FString filter);
};
