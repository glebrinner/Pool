
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SefaPool/Shared/SefaPoolGameInstance.h"
#include "PoolRemoteMessageBaseWidget.generated.h"


class UPoolRemoteMessagesWidget;

UCLASS()
class SEFAPOOL_API UPoolRemoteMessageBaseWidget : public UUserWidget
{
	GENERATED_BODY()
    
public:
    UPROPERTY(BlueprintReadOnly)
    UPoolRemoteMessagesWidget* parent;
    UPROPERTY(BlueprintReadOnly)
    int id;
	
    virtual void Parse(cool_json_t json);
    UFUNCTION(BlueprintCallable)
    void Dismiss();
    
    UFUNCTION(BlueprintImplementableEvent)
    void Init();
    
};
