// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OrientationManager.generated.h"

/**
 * 
 */


UENUM(BlueprintType)
enum class E_ScreenOrientation : uint8
{
    ORIENTATION_UNKNOWN                = 0 UMETA(DisplayName = "Unknown"),
    ORIENTATION_PORTRAIT               = 1 UMETA(DisplayName = "Portrait"),
    ORIENTATION_PORTRAIT_LOCKED        = 2 UMETA(DisplayName = "Portrait locked"),
    ORIENTATION_ANY_LANDSCAPE          = 3 UMETA(DisplayName = "Any landcape"),
    ORIENTATION_LANDSCAPE_LEFT_LOCKED  = 4 UMETA(DisplayName = "Left landscape"),
    ORIENTATION_LANDSCAPE_RIGHT_LOCKED = 5 UMETA(DisplayName = "Tight landscape"),
    ORIENTATION_UNLOCKED               = 6 UMETA(DisplayName = "Cushion")
};

UCLASS()
class SEFAPOOL_API UOrientationManager : public UObject
{
	GENERATED_BODY()
	
    float pcAspectRatio = 2.1f;
    
    int landscapeLongSide = 1000;
    int portraitLongSide = 1000;
    
public:
    UFUNCTION(BlueprintCallable)
    void SwitchToOrientation(E_ScreenOrientation orientation, APlayerController* pc);
};
