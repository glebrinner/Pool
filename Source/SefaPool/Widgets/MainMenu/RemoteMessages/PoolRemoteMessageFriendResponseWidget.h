
#pragma once

#include "CoreMinimal.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessageFriendResponseWidget.generated.h"

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageFriendResponseWidget : public UPoolRemoteMessageBaseWidget
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString username;
    UPROPERTY(BlueprintReadWrite)
    FString user_id;
    
    virtual void Parse(cool_json_t json);
};
