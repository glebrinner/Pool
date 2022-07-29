// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "LobbyPlayerState.h"
#include "../Shared/SharedDefs.h"
#include "../Shared/CheckConnectionPlayerController.h"
#include "../Shared/SefaPoolGameInstance.h"
#include "../Widgets/LoadingWidget.h"


#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "LobbyPlayerController.generated.h"

static const double MAX_PING_DELAY = 2;

UCLASS()
class SEFAPOOL_API ALobbyPlayerController : public ACheckConnectionPlayerController
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	UFUNCTION(Server, reliable)
		void Ping();

	UFUNCTION(Server, reliable)
		void CheckLastPing();

	bool IsPingDelayCorrect();

public:
	FString options;

private:
	double previous_ping = 0;
	double delta_ping = 10;
	UNetConnection* connect;
	FTimerHandle PingTimer;
	FTimerHandle CheckLastPingTimer;
	bool is_ping_delay_correct = false;
	double last_recieve_time = 0;
};
