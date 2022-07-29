
#pragma once

#include "CoreMinimal.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessageContestWinWidget.generated.h"

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageContestWinWidget : public UPoolRemoteMessageBaseWidget
{
	GENERATED_BODY()
	
    virtual void Parse(cool_json_t json);
};
