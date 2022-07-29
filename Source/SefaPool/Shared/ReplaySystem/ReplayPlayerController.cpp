// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayPlayerController.h"

#include "ReplayManager.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "Engine/DemoNetDriver.h"

AReplayPlayerController::AReplayPlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AReplayPlayerController::RestartRecording()
{}

void AReplayPlayerController::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(ObjectGatherer);
	ObjectGatherer->ballinfo->SetIsReplayMode(true);
	ObjectGatherer->cameraManager->ActivateTopCamera();
	SetViewTargetWithBlend(ObjectGatherer->cameraManager,0.5f,EViewTargetBlendFunction::VTBlend_Linear);	
}
