// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolSPPlayerController.h"
#include "PoolSPGameMode.h"
#include "../Shared/ObjectGatherer.h"

void APoolSPPlayerController::ServerPCReady_Implementation()
{
    if (!this || !objects->table)
        ERROR_DEBUG_PRINT_FUNC
        GetWorld()->GetAuthGameMode<APoolGameModeBase>()->PCReady(this);
}

bool APoolSPPlayerController::IsReady()
{
    return HasAuthority() || Super::IsReady();
}

