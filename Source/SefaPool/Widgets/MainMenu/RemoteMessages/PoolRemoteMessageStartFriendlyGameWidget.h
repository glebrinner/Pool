
#pragma once

#include "CoreMinimal.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessageStartFriendlyGameWidget.generated.h"

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageStartFriendlyGameWidget : public UPoolRemoteMessageBaseWidget
{
	GENERATED_BODY()
public:
    UPROPERTY()
    FString ip;
    UPROPERTY()
    int port;
    UPROPERTY(BlueprintReadOnly)
    FString username;
    
    virtual void Parse(cool_json_t json);
    
    UFUNCTION(BlueprintCallable)
    virtual void Travel();
};
