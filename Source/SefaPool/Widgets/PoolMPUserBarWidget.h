// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PoolUserBarWidgetBase.h"
#include "../8Ball/Pool8BallPlayerState.h"
#include "PoolMPUserBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API UPoolMPUserBarWidget : public UPoolUserBarWidgetBase
{
	GENERATED_BODY()
public:
    UPROPERTY(meta =(BindWidget), BlueprintReadOnly)
	class UTextBlock * SpecCountText;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* OpponentName;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
	UTextBlock* GameTimeText;

	void UpdateGameTime(int32 GameTime);
    
    UFUNCTION(BlueprintNativeEvent)
    void UpdateUserTargetMode(E_Pool8BallNextTarget nextMode);
    
    UFUNCTION(BlueprintNativeEvent)
    void UpdateOpponentTargetBalls(const TArray<int32>& nextBallsToHit);
    
    UFUNCTION(BlueprintNativeEvent)
    void UpdateOpponentTargetMode(E_Pool8BallNextTarget nextMode);

	void SetActivePlayer(uint8 PlayerNum);
	
	
};
