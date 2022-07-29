
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolSpectating.generated.h"

USTRUCT(BlueprintType)
struct FSpectatableMatchInfo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY(BlueprintReadOnly)
    FString ip;
    UPROPERTY(BlueprintReadOnly)
    FString port;
    UPROPERTY(BlueprintReadOnly)
    FString username1;
    UPROPERTY(BlueprintReadOnly)
    FString username2;
    bool valid;
    
};



UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolSpectating : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    
    UPROPERTY(BlueprintReadOnly)
    TArray<FSpectatableMatchInfo> current_spectatable_matches;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);

    UFUNCTION(BlueprintCallable)
    TArray<FSpectatableMatchInfo> GetMatchesSortedAndFiltered(
        bool onlyFriends, 
        bool byTime,
        bool ascending,
        FString filter);

};
