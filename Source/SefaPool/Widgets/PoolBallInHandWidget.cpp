// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolBallInHandWidget.h"
#include "../Shared/PoolPlayerControllerBase.h"



void UPoolBallInHandWidget::InitiateSwitchBallInHand()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->SwitchBallInHand();
}


void UPoolBallInHandWidget::ActivateButton_Implementation()
{
    
}
void UPoolBallInHandWidget::DeactivateButton_Implementation()
{
    
}
