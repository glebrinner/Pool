
#pragma once

#include "CoreMinimal.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessageChampionshipWinWidget.generated.h"

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageChampionshipWinWidget : public UPoolRemoteMessageBaseWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    int place;
    virtual void Parse(cool_json_t json);
};
