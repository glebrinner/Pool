
#pragma once

#include "CoreMinimal.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessageInviteToGameWidget.generated.h"

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageInviteToGameWidget : public UPoolRemoteMessageBaseWidget
{
	GENERATED_BODY()
public:
    UPROPERTY()
    FString invite_id;
    UPROPERTY()
    FString user_id;
    UPROPERTY(BlueprintReadOnly)
    FString username;
    UPROPERTY(BlueprintReadOnly)
    EDifficultyLevels  difficulty;
    UPROPERTY(BlueprintReadOnly)
    int amount;
    
    virtual void Parse(cool_json_t json);
    UFUNCTION(BlueprintCallable)
    virtual void AcceptInvitation();
    UFUNCTION(BlueprintCallable)
    virtual void RejectInvitation();
};
