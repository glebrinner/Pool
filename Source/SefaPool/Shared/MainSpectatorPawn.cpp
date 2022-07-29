// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Shared/MainSpectatorPawn.h"

#include "ObjectGatherer.h"


AMainSpectatorPawn::AMainSpectatorPawn()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMainSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
	// GATHER_OBJECT_GATHERER(ObjectGatherer);
	UE_LOG(LogTemp,Warning,TEXT("Spectator pawn AMainSpectatorPawn::BeginPlay called"))
	// SetCameraTo2D();
	// ObjectGatherer->gameflow->SetSpecPawn(this);
	// ObjectGatherer->gameflow->Spectating();
	// Init();
}

void AMainSpectatorPawn::Readjust()
{
	return;
}

// void AMainSpectatorPawn::Init()
// {
// 	APlayerController* PC = Cast<APlayerController>(GetController());
// 	if (PC)
// 	{
// 		APoolHUD* HUD = Cast<APoolHUD>(PC->GetHUD());
// 		if (HUD)
// 		{
// 			HUD->ActivateSpectatingUI();
// 			UE_LOG(LogTemp,Error,TEXT("Activate spectating UI"))
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp,Error,TEXT("HUD NOT VALID"))
// 		}
// 	}
// 	else
// 	{
// 		UE_LOG(LogTemp,Error,TEXT("PC not valid"))
// 	}
// 	
// }
//
// void AMainSpectatorPawn::InitiateSwitch2D()
// {
// 	UE_LOG(LogTemp,Error,TEXT("AMainSpectatorPawn::InitiateSwitch2D"))
// }
//
// void AMainSpectatorPawn::InitiateSwitch3D()
// {
// 	UE_LOG(LogTemp,Error,TEXT("AMainSpectatorPawn::InitiateSwitch3D"))
// }
//
// void AMainSpectatorPawn::RegisterJoystickMovement(FVector2D newPos)
// {
// 	UE_LOG(LogTemp,Error,TEXT("AMainSpectatorPawn::RegisterJoystickMovement"))
// }

// void AMainSpectatorPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
// {
// 	Super::SetupPlayerInputComponent(PlayerInputComponent);
// 	PlayerInputComponent->BindAction("ChangeCamera",EInputEvent::IE_Pressed,this,&AMainSpectatorPawn::SetCameraTo2D);
// }

// void AMainSpectatorPawn::SetCameraTo2D()
// {
// 	UE_LOG(LogTemp,Error,TEXT("AMainSpectatorPawn::SetCameraTo2D"))
// 	if (ObjectGatherer && ObjectGatherer->cameraManager)
// 	{
// 		APlayerController* OwnerController = Cast<APlayerController>(GetOwner());
// 		if (OwnerController)
// 		{
// 			ObjectGatherer->cameraManager->ActivateTopCamera();
// 			OwnerController->ClientSetViewTarget(ObjectGatherer->cameraManager);
// 		}
// 	}
// }

