// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolPowerBarChalkWidget.h"
#include "../Shared/PoolPlayerControllerBase.h"


void UPoolPowerBarChalkWidget::SetPower(float power)
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->SetPower(power);
}
void UPoolPowerBarChalkWidget::InitiateShot()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->OnPlay();
}
void UPoolPowerBarChalkWidget::InitiateChalking()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->ApplyChalk();
}

void UPoolPowerBarChalkWidget::SwitchAngle()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->SwitchAngle();
}

void UPoolPowerBarChalkWidget::PowerSliderGrabbed()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->PowerChangeStarted();
}

void UPoolPowerBarChalkWidget::PowerSliderUngrabbed()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->PowerChangeEnded();
}



void UPoolPowerBarChalkWidget::Update_Implementation(int shotsUntilRechalking)
{
    
}

void UPoolPowerBarChalkWidget::ActivateAngleButton_Implementation()
{
    
}

void UPoolPowerBarChalkWidget::DeactivateAngleButton_Implementation()
{
    
}
