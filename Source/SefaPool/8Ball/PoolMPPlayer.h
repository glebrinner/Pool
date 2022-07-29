// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Shared/PoolPlayer.h"
#include "PoolMPPlayer.generated.h"

/**
 * 
 */
UCLASS()
class SEFAPOOL_API APoolMPPlayer : public APoolPlayer
{
	GENERATED_BODY()
    
    FTimerHandle uploadTimer;
    
public:
    
    APoolMPPlayer();
    
    virtual void Activate();
    virtual void Deactivate();
    
    void TimedSend();
};
