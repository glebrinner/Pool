
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolContests.generated.h"

UENUM(BlueprintType)
enum class EContestType : uint8
{
    CONTEST_NONE            = 0 UMETA(DisplayName = "Not selected"),
    CONTEST_SPEED_POOL      = 1 UMETA(DisplayName = "Speed pool"),
    CONTEST_LINE_UP         = 2 UMETA(DisplayName = "Line up"),
    CONTEST_FBNL            = 3 UMETA(DisplayName = "Firm break no limit"),
    CONTEST_CIRCLE_OF_BALLS = 4 UMETA(DisplayName = "Circle of balls"),
    CONTEST_CUSHION         = 5 UMETA(DisplayName = "Cushion")
};


USTRUCT(BlueprintType)
struct FTrainingContestResult
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "ContestResult")
    int balls;
    UPROPERTY(BlueprintReadWrite, Category = "ContestResult")
    int time;
    UPROPERTY(BlueprintReadWrite, Category = "ContestResult")
    FString username;
};


USTRUCT(BlueprintType)
struct SEFAPOOL_API FTrainingContestInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    EContestType type_id;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    uint8 division;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    uint8 instance_id;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    int fee;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    FDateTime ends_localTime;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    bool has_user_participated;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    FTrainingContestResult user_best_result;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    TArray<FTrainingContestResult> best_instance_results;
    UPROPERTY(BlueprintReadWrite, Category = "ContestInfo")
    FTrainingContestResult world_record;
};


UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolContests : public USefaPoolDataBlock
{
    GENERATED_BODY()
    
    UPROPERTY()
    TArray<FTrainingContestInfo> active_contests_;
    UPROPERTY()
    TMap<int, FTrainingContestInfo> active_contests_processing;
    
    int completed_async_endpoints;
    int targeted_async_endpoints;
    
public:
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    
    void Update_RequestActiveContests(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                      const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    void Update_RequestShortLeaderboards(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                         const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    void Update_RequestUserResults(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    void Update_RequestBestResults(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    void Update_Finalize(const std::function<void(USefaPoolDataBlock*)>&& successful,
                         const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    
    UFUNCTION(BlueprintCallable)
    void Participate(int id_);
    
    UFUNCTION(BlueprintCallable)
    FTrainingContestInfo GetActiveContestByType(EContestType type,
                                                int division);
};

