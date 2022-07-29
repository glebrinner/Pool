// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AgonesComponent.h" 
#include "../Shared/TcpComunicationActor.h"
#include "PoolServerGameInstance.h"
#include "PoolServerMatchSelectorGameMode.generated.h"


UCLASS()
class SEFAPOOL_API APoolServerMatchSelectorGameMode : public AGameModeBase
{
	GENERATED_BODY()

    //ATcpComunicationActor* TcpSocket;
    UAgonesComponent* AgonesSDK;
    
public:
    APoolServerMatchSelectorGameMode();
    virtual void BeginPlay();
    UFUNCTION()
    void GameServerCallback(const FGameServerResponse& gameServerInfo);
    
    UFUNCTION()
    void ReadyAgones(const FGameServerResponse& Response);

    UFUNCTION()
    void AgonesSuccess(const FEmptyResponse& Response);

    UFUNCTION()
    void AgonesError(const FAgonesError& Response);

};

