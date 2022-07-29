
#pragma once

#include "CoreMinimal.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessageFriendRequestWidget.generated.h"

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageFriendRequestWidget : public UPoolRemoteMessageBaseWidget
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString username;
    UPROPERTY(BlueprintReadWrite)
    FString user_id;
    
    
    UFUNCTION(BlueprintCallable)
    void AcceptFriendRequest();
    UFUNCTION(BlueprintCallable)
    void IgnoreFriendRequest();
    
    virtual void Parse(cool_json_t json);
};
