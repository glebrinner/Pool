
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolChampionship.generated.h"

USTRUCT(BlueprintType)
struct FChampionshipEntry
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(BlueprintReadOnly)
    int tokens_won;
    UPROPERTY(BlueprintReadOnly)
    FString username;
    
};

void from_json(const cool_json_t& json, FChampionshipEntry& entry);

USTRUCT(BlueprintType)
struct SEFAPOOL_API FChampionshipDivision
{
    GENERATED_USTRUCT_BODY()
    UPROPERTY(BlueprintReadOnly)
    TArray<FChampionshipEntry> leaderboard;
    UPROPERTY(BlueprintReadOnly)
    int pos;
    UPROPERTY(BlueprintReadOnly)
    FDateTime endTime;
};

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolChampionship : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly, Category = "Championship")
    TMap<int, FChampionshipDivision> leaderboards;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
};
