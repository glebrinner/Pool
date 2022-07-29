// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerVarificationGameMode.h"

void APlayerVarificationGameMode::BeginPlay()
{
	Super::BeginPlay();
	Database = dynamic_cast<ADatabaseActor*>((GetWorld()->SpawnActor<ADatabaseActor>(FVector(0, 0, 0), FRotator(0, 0, 0))));
	if (Database == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor pointer is null"));
	}
	Database->PrimaryActorTick.TickInterval = 1;
	TcpSocket = dynamic_cast<ATcpComunicationActor*>((GetWorld()->SpawnActor<ATcpComunicationActor>(FVector(0, 0, 0), FRotator(0, 0, 0))));
	TcpSocket->set_handle_query([=](uint8* ReceivedData, uint32 Size) { Database->handle_query(ReceivedData, Size); });
	TcpSocket->StartTCPReceiver(FString("DBTcpSocket"), FString("0.0.0.0"), 2590);
	GetWorldTimerManager().SetTimer(SendVerifyTimer, this,
		&APlayerVarificationGameMode::SendVerify, 30, true, 60.0f);
}

void APlayerVarificationGameMode::PreLogin(const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (!Database->find(UGameplayStatics::ParseOption(Options, "access_token"),
		UGameplayStatics::ParseOption(Options, "player_id")))
	{
		ErrorMessage = FString("Authentification failed.\nAccess token: ") + UGameplayStatics::ParseOption(Options, "access_token") + FString("\nUser id:") + UGameplayStatics::ParseOption(Options, "player_id");
	}
}

void APlayerVarificationGameMode::SendVerify()
{
	Database->SendVerify();
}
