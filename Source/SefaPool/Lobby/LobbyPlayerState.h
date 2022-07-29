// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Engine/World.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"
#include "../Widgets/LoadingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyPlayerState.generated.h"


static const int MAX_WAITING_TIME = 10;

UCLASS()
class SEFAPOOL_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

	UFUNCTION(Client, reliable)
	void ClientRPCFunction(int stake_, int difficulty_, int user_id_);

	UFUNCTION(Client, reliable)
	void TravelToPoolGame(const FString& ip, int32 port);

	//void WritePlayerData(FString Options);
	void UpdateTime();

	void WritePlayerData(FString options);

public:
	int stake = 10;
	int difficulty = 10;
	FString access_token;
	int user_id = -1;

private:
	float waiting_time_begin;
	FTimerHandle WaitingTimer;
};
