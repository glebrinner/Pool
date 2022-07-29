// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundEngine.h"
#include "ObjectGatherer.h"
#include "../8Ball/BallInfo.h"
#include "ReplaySystem/ReplayManager.h"

// Sets default values
ASoundEngine::ASoundEngine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASoundEngine::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(objects)
}


void ASoundEngine::EmitSound(E_PoolSounds sound, FVector location, float power)
{
    switch (sound)
    {
        case E_PoolSounds::CUE_HIT:
            EmitCueHitSound(location, power);
            break;
            
        case E_PoolSounds::BALL_TO_BALL:
            EmitBallToBallHitSound(location, power);
            break;
            
            
        case E_PoolSounds::DROP:
            EmitDropSound(location, power);
            break;
            
            
        case E_PoolSounds::CUSHION:
            EmitCushionSound(location, power);
            break;
        default:
            break;
    }
}

void ASoundEngine::EmitSound_ClientOrServer(E_PoolSounds sound, FVector location, float power)
{
    // todo сохранять
    if (objects->ballinfo->bShouldGetInfo && HasAuthority())
    {
        FSoundInfo soundInfoInstance;
        soundInfoInstance.pos = location;
        soundInfoInstance.power = power;
        soundInfoInstance.sound = sound;
        soundInfoInstance.time = objects->shotinfo->GetTimeSinceShotBegan();
        objects->ballinfo->SoundsInfoArray.Add(soundInfoInstance);
        objects->ReplayManager->AppendSoundInfo(soundInfoInstance);
    }
    else
    {
        PRINTF((TEXT("Playing sound")), Green)
        EmitSound(sound, location, power);
    }
}
