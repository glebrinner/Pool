// Fill out your copyright notice in the Description page of Project Settings.


#include "VarificationGameMode.h"

void AVarificationGameMode::BeginPlay()
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
	//GetWorldTimerManager().SetTimer(SendVerifyTimer, this,
	//	&AVarificationGameMode::SendVerify, 30, true, 60.0f);
}

void AVarificationGameMode::SetVerification(bool varification_enable) {
	varification_enable_ = varification_enable;
}

void AVarificationGameMode::SendVerify()
{
	Database->SendVerify();
}

void AVarificationGameMode::PreLogin(const FString& Options,
	const FString& Address,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
	if (varification_enable_ && !Database->find(UGameplayStatics::ParseOption(Options, "access_token"),
		UGameplayStatics::ParseOption(Options, "player_id")))
	{
		ErrorMessage = FString("Authentification failed.\nAccess token: ") + UGameplayStatics::ParseOption(Options, "access_token") + FString("\nUser id:") + UGameplayStatics::ParseOption(Options, "player_id");
	}
}

APlayerController* AVarificationGameMode::Login(UPlayer* NewPlayer,
	ENetRole InRemoteRole,
	const FString& Portal,
	const FString& Options,
	const FUniqueNetIdRepl& UniqueId,
	FString& ErrorMessage)
{
	APlayerController* pc = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	if (pc) {
		UE_LOG(LogTemp, Warning, TEXT("PC from login is valid"));
		UE_LOG(LogTemp, Warning, TEXT("Name of PC %s"), *pc->GetClass()->GetName());
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("PC from login isn't valid"));
	}
	UE_LOG(LogTemp, Warning, TEXT("Given Options: %s"), *Options);
	if (Cast<AVarificationPlayerController>(pc)) {
		UE_LOG(LogTemp, Warning, TEXT("PC after cast is valid Saving options: %s"), *Options);
		Cast<AVarificationPlayerController>(pc)->Options = Options;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("PC after cast isn't valid"));
	}
	return pc;
}

void AVarificationGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (Cast<AVarificationPlayerController>(NewPlayer))
		Cast<AVarificationPlayerController>(NewPlayer)->InitPlayerStateData();
}