// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataAsset.h"
#include "Engine/Texture.h"
#include "Engine/Texture2D.h"
#include "BallData.generated.h"

USTRUCT(BlueprintType)
struct FMovingBallData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSoftObjectPtr<UTexture> BallTexture;
};

UCLASS(BlueprintType)
class SEFAPOOL_API UBallData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FMovingBallData> MovingBallData;
};

