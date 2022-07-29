// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolMPPlayer.h"
#include "../Shared/SharedDefs.h"
#include "Pool8BallPlayerController.h"
#include "Pool8BallPlayerState.h"

APoolMPPlayer::APoolMPPlayer() :
    APoolPlayer()
{
    
}

void APoolMPPlayer::Activate()
{
    Super::Activate();
    
    GetWorldTimerManager().SetTimer(uploadTimer, this, &APoolMPPlayer::TimedSend, 0.5f, true, 1.0f);
    
}
void APoolMPPlayer::Deactivate()
{
    Super::Deactivate();
    GetWorldTimerManager().ClearTimer(uploadTimer);
}

void APoolMPPlayer::TimedSend()
{
    APool8BallPlayerController* pc = dynamic_cast<APool8BallPlayerController*> (UGameplayStatics::GetPlayerController(this, 0));
    if (!pc)
        return;
    APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*> (pc->PlayerState);
    pc->UploadOpponentInfo(ps->cueInfo.GetCueNum(), GetActorLocation(), GetActorRotation(), cue->GetRelativeLocation(), cue->GetRelativeRotation());
}
