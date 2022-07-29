// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "VarificationPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API AVarificationPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	FString user_id;
};
