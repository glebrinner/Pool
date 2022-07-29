// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolUserBarWidgetBase.h"
#include "../Shared/PoolPlayerControllerBase.h"



void UPoolUserBarWidgetBase::ShowShop()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->ShowShop();
}

void UPoolUserBarWidgetBase::UpdateUserTargetBalls_Implementation(const TArray<int32>& nextBallsToHit)
{
}

void UPoolUserBarWidgetBase::UpdateUsername_Implementation(const FString& username, bool opponent)
{}

void UPoolUserBarWidgetBase::StartingAiming_Implementation()
{}

void UPoolUserBarWidgetBase::ReplicationPauseStart_Implementation()
{}

void UPoolUserBarWidgetBase::ReplicationPauseEnd_Implementation()
{}

void UPoolUserBarWidgetBase::Shooting_Implementation()
{}

void UPoolUserBarWidgetBase::BallsStopped_Implementation()
{}
