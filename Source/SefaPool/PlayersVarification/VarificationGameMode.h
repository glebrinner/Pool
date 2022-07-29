// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VarificationPlayerController.h"
#include "SefaPool/Shared/TcpComunicationActor.h"
#include "VarificationGameMode.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API AVarificationGameMode : public AGameModeBase
{
	GENERATED_BODY()
protected:
	FTimerHandle SendVerifyTimer;
	ADatabaseActor* Database;
	ATcpComunicationActor* TcpSocket;

public:

	virtual void BeginPlay() override;

	void SendVerify();

	void SetVerification(bool varification_enable);

	virtual void PreLogin(const FString& Options,
		const FString& Address,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;

	virtual APlayerController* Login(UPlayer* NewPlayer,
		ENetRole InRemoteRole,
		const FString& Portal,
		const FString& Options,
		const FUniqueNetIdRepl& UniqueId,
		FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	bool varification_enable_ = false;
};
