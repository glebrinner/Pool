// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolMPUserBarWidget.h"

#include "Components/TextBlock.h"


void UPoolMPUserBarWidget::UpdateUserTargetMode_Implementation(E_Pool8BallNextTarget nextMode)
{
    
}
void UPoolMPUserBarWidget::UpdateOpponentTargetBalls_Implementation(const TArray<int32>& nextBallsToHit)
{
    
}
void UPoolMPUserBarWidget::UpdateOpponentTargetMode_Implementation(E_Pool8BallNextTarget nextMode)
{
    
}

void UPoolMPUserBarWidget::UpdateGameTime(int32 GameTime)
{
	FString Minutes = ("00" + FString::FromInt(GameTime / 60)).Right(2);
	FString Seconds = ("00" + FString::FromInt(GameTime % 60)).Right(2);
	GameTimeText->SetText(FText::FromString(Minutes + ":" + Seconds));
}

void UPoolMPUserBarWidget::SetActivePlayer(uint8 PlayerNum)
{

	UE_LOG(LogTemp,Error,TEXT("Set active player %i"),PlayerNum)
	UTextBlock* ActivePlayerTextBlock = PlayerNum == 0 ? PlayerName : OpponentName;
	UTextBlock* DefaultPlayerTextBlock = PlayerNum == 0 ? OpponentName : PlayerName;
	FSlateColor ActivePlayerColor = FSlateColor(FLinearColor(FVector(1.f,0.f,0.f)));
	FSlateColor DefaultPlayerColor = FSlateColor(FLinearColor(FVector(0.f)));
	ActivePlayerTextBlock->SetColorAndOpacity(ActivePlayerColor);
	DefaultPlayerTextBlock->SetColorAndOpacity(DefaultPlayerColor);
}
