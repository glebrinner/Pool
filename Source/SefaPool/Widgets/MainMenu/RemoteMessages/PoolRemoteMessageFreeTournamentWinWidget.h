
#pragma once

#include "CoreMinimal.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessageFreeTournamentWinWidget.generated.h"

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageFreeTournamentWinWidget : public UPoolRemoteMessageBaseWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    int amount;
	
    virtual void Parse(cool_json_t json);
};
