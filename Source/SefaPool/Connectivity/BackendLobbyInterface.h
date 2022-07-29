

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "BackendLobbyInterface.generated.h"


UCLASS(BlueprintType)
class SEFAPOOL_API UBackendLobbyInterface : public UObject
{
    GENERATED_BODY()
    USefaPoolGameInstance* gi;
    FString ip;
    int port;
    
public:
    
    void Init(USefaPoolGameInstance* gi_);
    
    UFUNCTION(BlueprintCallable)
    void Update();
    UFUNCTION(BlueprintCallable)
    FString GetLobbyTravelAddress();
    
};
