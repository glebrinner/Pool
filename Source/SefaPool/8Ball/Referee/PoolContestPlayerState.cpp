// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolContestPlayerState.h"

void APoolContestPlayerState::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME( APoolContestPlayerState, pocketedBallsCount );
    DOREPLIFETIME( APoolContestPlayerState, inGameTime );
}


bool APoolContestPlayerState::CanTargetThisBall(int n)
{
    return true;
}
