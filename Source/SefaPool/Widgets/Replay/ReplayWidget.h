// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Widgets/PoolUserBarWidgetBase.h"
#include "ReplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UReplayWidget : public UPoolUserBarWidgetBase
{
	GENERATED_BODY()
	AObjectGatherer* ObjectGatherer;

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* FirstPlayerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SecondPlayerName;

	UPROPERTY(meta = (BindWidget))
	class UButton* StartPlayReplayButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* GameTimeText;

	UPROPERTY(meta = (BindWidget))
	UButton* AutoPlayButton;

	void NativeConstruct() override;

	bool bIsReplayPaused;

	bool bAutoPlayActivated;

	UFUNCTION()
	void OnAutoPlayButtonClicked();
	
	UFUNCTION()
	void OnPlayReplayButtonClicked();

public:
	void SetPlayersNickNames(FString FirstNick, FString SecondNick = "");

	void SetActivePlayer(uint8 PlayerNum);

	void SetGameTime(int32 NewTime);
};
