// Fill out your copyright notice in the Description page of Project Settings.


#include "Pool8BallPlayerState.h"
#include "../Shared/PoolGameStateBase.h"
#include "Engine/World.h"

APool8BallPlayerState::APool8BallPlayerState() : APoolPlayerStateBase(),
	nextTarget                 (E_Pool8BallNextTarget::OPEN_TABLE),
	
	currentFrameNextBallsToHit (),
	currentFramePocketed       ()
{
}

void APool8BallPlayerState::UpdatePocketed()
{
	ChangeNextTarget(nextTarget);
	if (currentFrameNextBallsToHit.Num() == 0 && 
		(nextTarget == E_Pool8BallNextTarget::LOW_BALLS || nextTarget == E_Pool8BallNextTarget::HIGH_BALLS))
	{
		if (currentFramePocketed.Num() == 7)
		{
			ChangeNextTarget(E_Pool8BallNextTarget::BLACK_BALL);
		}
		else
			ERROR_DEBUG_PRINT_FUNC_INFO("Broken ball balance")
	}
}

void APool8BallPlayerState::ChangeNextTarget(E_Pool8BallNextTarget target)
{
	currentFrameNextBallsToHit.Empty();
	currentFramePocketed.Empty();
    TArray<uint8> targetable;
	switch (target)
	{
	case E_Pool8BallNextTarget::OPEN_TABLE:
        {
            for (int i = 1; i < 16; i++)
                if (i != 8)
                    targetable.Add(i);
            break;
        }
	case E_Pool8BallNextTarget::LOW_BALLS:
        {
            bool* ballsInGame = GetWorld()->GetGameState<APoolGameStateBase>()->ballsInGame;
            for (int i = 1; i < 8; i++)
            {
                if (ballsInGame[i]) currentFrameNextBallsToHit.Add(i);
                else currentFramePocketed.Add(i);
            }
            break;
        }
	case E_Pool8BallNextTarget::HIGH_BALLS:
        {
            bool* ballsInGame = GetWorld()->GetGameState<APoolGameStateBase>()->ballsInGame;
            for (int i = 9; i < 16; i++)
            {
                if (ballsInGame[i]) currentFrameNextBallsToHit.Add(i);
                else currentFramePocketed.Add(i);
            }
            break;
        }
	case E_Pool8BallNextTarget::BLACK_BALL:
        {
            currentFrameNextBallsToHit.Add(8);
            break;
        }
	}
	nextTarget = target;
    
    for (auto ball_ : currentFrameNextBallsToHit)
        targetable.Add(ball_);
    ClientSetTargetableBalls(targetable);
	//123 PRINTF((TEXT("New target %d"), target), Red)

}
/*
void APool8BallPlayerState::ServerForceReplicate_Implementation()
{
    if (!HasAuthority()) return;
	ClientForceReplicate(nextTarget, ballInHand, currentFrameNextBallsToHit, currentFramePocketed);
}
void APool8BallPlayerState::ClientForceReplicate_Implementation(E_Pool8BallNextTarget nextTarget_, bool ballInHand_, const TArray<int>& currentFrameNextBallsToHit_, const TArray<int>& currentFramePocketed_)
{
	nextTarget = nextTarget_;
	ballInHand = ballInHand_;
	currentFrameNextBallsToHit = currentFrameNextBallsToHit_;
	currentFramePocketed = currentFramePocketed_;
}
*/

E_Pool8BallNextTarget APool8BallPlayerState::GetNextTarget() const
{
	return nextTarget;
}

bool APool8BallPlayerState::IsBallInHand() const
{
	return ballInHand;
}
void APool8BallPlayerState::CurrentFrameNextBallsToHit(TArray<int32>& arr) const
{
    if (!this) return;
    if (ballInHand)
        PRINTF((TEXT("PS: FOUL")), Red)
	arr = currentFrameNextBallsToHit;
}
void APool8BallPlayerState::CurrentFramePocketed(TArray<int32>& arr ) const
{
	arr = currentFramePocketed;

}

void APool8BallPlayerState::UpdateClient()
{
    Super::UpdateClient();
}

void APool8BallPlayerState::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME( APool8BallPlayerState, nextTarget );
    DOREPLIFETIME( APool8BallPlayerState, ballInHand );
    DOREPLIFETIME( APool8BallPlayerState, currentFrameNextBallsToHit );
    DOREPLIFETIME( APool8BallPlayerState, currentFramePocketed );
}

bool APool8BallPlayerState::CanTargetThisBall(int n)
{
    // return nextTarget == E_Pool8BallNextTarget::OPEN_TABLE || currentFrameNextBallsToHit.Contains(n);
	return Super::CanTargetThisBall(n);
}

