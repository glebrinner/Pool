// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "../Shared/SharedDefs.h"
#include "../Shared/PoolPlayerControllerBase.h"
#include "PoolSPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APoolSPPlayerController : public APoolPlayerControllerBase
{
	GENERATED_BODY()
public:
    virtual void ServerPCReady_Implementation() override;
    virtual bool IsReady();
};
