
#pragma once

#include "SefaPoolDataBlock.h"
#include "SefaPoolLobby.generated.h"


UCLASS(BlueprintType)
class SEFAPOOL_API USefaPoolLobby : public USefaPoolDataBlock
{
    GENERATED_BODY()
    FString ip;
    int port;
public:
    virtual void Save();
    
    virtual void Load();
    
    virtual void PullWithCallbacks(const std::function<void(USefaPoolDataBlock*)>&& successful,
                                   const std::function<void(USefaPoolDataBlock*)>&& unsuccessful);
    UFUNCTION(BlueprintCallable)
    FString GetLobbyTravelAddress();
};
