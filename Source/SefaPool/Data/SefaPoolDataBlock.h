
#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/SaveGame.h"
#include "SefaPool/Shared/HTTP_GI.h"
#include "SefaPoolDataBlock.generated.h"
UENUM(BlueprintType)
enum class EConsumableType : uint8
{
    CUE,
    CHALK,
    CLOTH,
    TOKENPAK,
    BUNDLE
};

class USefaPoolGameInstance;
UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolDataBlock : public USaveGame
{
    GENERATED_BODY()
protected:
    USefaPoolGameInstance* gi;
public:
    virtual void Init(USefaPoolGameInstance* gi_);
    
    UFUNCTION(BlueprintCallable)
    virtual void Save();
    
    UFUNCTION(BlueprintCallable)
    virtual void Load();
    
    UFUNCTION(BlueprintCallable)
    virtual void Pull();
    
    UFUNCTION(BlueprintCallable)
    virtual void PullWithDelegates(const FResponseDelegat& succesful,
                                   const FResponseDelegat& unsuccessful);
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& succesful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    
};
