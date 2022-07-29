// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TcpComunicationActor.h"
#include "PlayerVarificationGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APlayerVarificationGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	FTimerHandle SendVerifyTimer;
	ADatabaseActor* Database;
	ATcpComunicationActor* TcpSocket;

public:

	virtual void BeginPlay() override;
	
	void SendVerify();

	virtual void PreLogin(const FString& Options,
		const FString& Address,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;

};
