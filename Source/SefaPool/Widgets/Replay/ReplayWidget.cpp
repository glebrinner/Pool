// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Widgets/Replay/ReplayWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"

void UReplayWidget::NativeConstruct()
{
	Super::NativeConstruct();
	StartPlayReplayButton->OnClicked.AddDynamic(this,&UReplayWidget::OnPlayReplayButtonClicked);
	AutoPlayButton->OnClicked.AddDynamic(this,&UReplayWidget::OnAutoPlayButtonClicked);
	bIsReplayPaused = true;
	bAutoPlayActivated = true;
	GATHER_OBJECT_GATHERER(ObjectGatherer);
}

void UReplayWidget::OnAutoPlayButtonClicked()
{
	bAutoPlayActivated = bAutoPlayActivated ? false : true;
	ObjectGatherer->ballvisualization->SetAutoPlayReplay(bAutoPlayActivated);
	if (bAutoPlayActivated && !ObjectGatherer->ballvisualization->IsVisualize())
	{
		ObjectGatherer->ballvisualization->ReinitToNextStage();
		UE_LOG(LogTemp,Error,TEXT("Activate autoreplay visualization, and reinit to next stage"))
	}
	UE_LOG(LogTemp,Error,TEXT("Auto play replay state changed"))
}

void UReplayWidget::OnPlayReplayButtonClicked()
{
	ObjectGatherer->ReplayManager->PlayReplay();
	StartPlayReplayButton->SetIsEnabled(false);
	UE_LOG(LogTemp,Error,TEXT("Play replay button clicked"))
}

void UReplayWidget::SetPlayersNickNames(FString FirstNick, FString SecondNick)
{
	FirstPlayerName->SetText(FText::FromString(FirstNick));
	UE_LOG(LogTemp,Error,TEXT("first player name is %s"),*FirstNick)
	if (SecondNick.IsEmpty())
	{
		SecondPlayerName->SetVisibility(ESlateVisibility::Collapsed);
		UE_LOG(LogTemp,Error,TEXT("second player name is empty"))
	}
	else
	{
		SecondPlayerName->SetText(FText::FromString(SecondNick));
	}
	UE_LOG(LogTemp,Error,TEXT("Set player nick names"))
}

void UReplayWidget::SetActivePlayer(uint8 PlayerNum)
{
	UTextBlock* ActivePlayerTextBLock = PlayerNum == 0 ? FirstPlayerName : SecondPlayerName;
	UTextBlock* InActivePlayerTextBlock = PlayerNum == 0 ? SecondPlayerName : FirstPlayerName;
	FSlateColor ActiveColor = FSlateColor(FLinearColor(FVector(1.f)));
	FSlateColor DefaultColor = FSlateColor(FLinearColor(FVector(1.f,0.f,0.f)));
	ActivePlayerTextBLock->SetColorAndOpacity(ActiveColor);
	InActivePlayerTextBlock->SetColorAndOpacity(DefaultColor);
	UE_LOG(LogTemp,Error,TEXT("Set active player"))
}

void UReplayWidget::SetGameTime(int32 NewTime)
{
	UE_LOG(LogTemp,Error,TEXT("Set game time %i"),NewTime)
	FString Minutes = ("00" + FString::FromInt(NewTime / 60)).Right(2);
	FString Seconds = ("00" + FString::FromInt(NewTime % 60)).Right(2);
	GameTimeText->SetText(FText::FromString(Minutes + ":" + Seconds));
}
