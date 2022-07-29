
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolStats.generated.h"


USTRUCT(BlueprintType)
struct SEFAPOOL_API FUserStats
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 games_won;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 games_lost;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 firm_breaks;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 firm_replays;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float reaction_time;
    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    int32 tournaments_won;
};

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolStats : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    FUserStats data;
    
    UPROPERTY(BlueprintReadOnly)
    FUserStats ext_stats;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    UFUNCTION(BlueprintCallable)
    void PullStatsForUser(const FString& user_id,
                          const FResponseDelegat& successful,
                          const FResponseDelegat& unsuccessful);
};
