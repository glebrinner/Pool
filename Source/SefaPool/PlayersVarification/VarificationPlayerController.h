// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VarificationPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/NetConnection.h"
#include "VarificationPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API AVarificationPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void BeginPlay() override;
	
	void SetDelay(double delay);
	double GetDelay() const;
	void CloseConnection();
	bool IsClientAlive() const;

	virtual void InitPlayerStateData();

	FString Options;
protected:
	double delay_ = 20;
	UNetConnection* connection_;
	
};