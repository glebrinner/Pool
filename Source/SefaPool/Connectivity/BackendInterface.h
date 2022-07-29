

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "BackendLobbyInterface.h"
#include "BackendInterface.generated.h"


UCLASS(BlueprintType)
class SEFAPOOL_API UBackendInterface : public UObject
{
    GENERATED_BODY()
    USefaPoolGameInstance* gi;
public:
    
    UPROPERTY(BlueprintReadWrite)
    UBackendLobbyInterface* lobby;
    
    void Init(USefaPoolGameInstance* gi_);
    
};
