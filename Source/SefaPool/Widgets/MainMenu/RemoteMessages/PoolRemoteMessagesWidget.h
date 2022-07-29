// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Widgets/PoolBaseWidget.h"
#include "SefaPool/Data/SefaPoolMessages.h"
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessagesWidget.generated.h"

USTRUCT(BlueprintType)
struct FMessageWidgetInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite)
    UUserWidget* widget;
    UPROPERTY(BlueprintReadWrite)
    FString shortDescription;
    UPROPERTY(BlueprintReadWrite)
    bool read;
    UPROPERTY(BlueprintReadWrite)
    int id;
};

UCLASS()
class SEFAPOOL_API UPoolRemoteMessagesWidget : public UPoolBaseWidget
{
	GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MessageClasses")
    TMap<ERemoteMessageType, TSubclassOf<UPoolRemoteMessageBaseWidget>> messageWidgetClasses;
    
    /*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MessageWidgets")
    TArray<UPoolRemoteMessageBaseWidget*> messageWidgets;*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MessageWidgets")
    TArray<FMessageWidgetInfo> messageWidgets;
	UFUNCTION(BlueprintCallable)
    void Update();
    
    UFUNCTION(BlueprintImplementableEvent, Category = "Relink")
    void RelinkMessageWidgets();

    UFUNCTION(BlueprintCallable)
    void OpenSpecificMessageWidget(int id);

    UFUNCTION(BlueprintImplementableEvent)
    void ShowSpecificMessageWidget(UPoolRemoteMessageBaseWidget* widget);

};
