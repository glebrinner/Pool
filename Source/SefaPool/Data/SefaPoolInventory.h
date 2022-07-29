
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolInventory.generated.h"

UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolInventory : public USefaPoolDataBlock
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
    TArray<int> cue_durabilities;
    UPROPERTY(BlueprintReadWrite, Category = "Cue")
    int current_cue;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
    TArray<int> chalk_durabilities;
    UPROPERTY(BlueprintReadWrite, Category = "Chalk")
    int current_chalk;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
    TArray<bool> cloth_ownership;
    UPROPERTY(BlueprintReadWrite, Category = "Cloth")
    int current_cloth;
    UPROPERTY(BlueprintReadWrite, Category = "Bundle")
    TArray<bool> bundle_ownership;
    UPROPERTY(BlueprintReadWrite)
    int64 balance;
    
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
};
