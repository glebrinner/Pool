// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/NetConnection.h"
#include "CheckConnectionPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API ACheckConnectionPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void BeginPlay() override;
	void SetDelay(double delay);
	double GetDelay() const;
	void CloseConnection();
	bool IsClientAlive() const; 

protected:
	double delay_ = 20;
	UNetConnection* connection_;
};
