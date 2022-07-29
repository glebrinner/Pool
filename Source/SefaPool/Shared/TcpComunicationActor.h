// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "Networking.h"
#include "Sockets.h"
#include "CoreMinimal.h"
#include "DatabaseActor.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "TcpComunicationActor.generated.h"

UCLASS()
class SEFAPOOL_API ATcpComunicationActor : public AActor
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;
	FTimerHandle ConnectionTimer;
	FTimerHandle ListenerTimer;

	void set_handle_query(std::function<void(uint8* ReceivedData, uint32 Size)> handle_query);

	bool StartTCPReceiver(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort
	);

	bool CreateTCPConnectionListener(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort,
		const int32 ReceiveBufferSize = 2 * 1024 * 1024
	);

	void TCPConnectionListener(); 	//can thread this eventually
	void TCPSocketListener();		//can thread this eventually

	FString StringFromBinaryArray(uint8* BinaryArray);

private:
	std::function<void(uint8* ReceivedData, uint32 Size)> handle_query_;
};


