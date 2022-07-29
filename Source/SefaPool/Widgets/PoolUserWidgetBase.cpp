// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolUserWidgetBase.h"




void UPoolUserWidgetBase::EnterDisabledMode_Implementation()
{
    
}
void UPoolUserWidgetBase::EnterEnabledMode_Implementation()
{
    
}

bool UPoolUserWidgetBase::TouchStarted_Custom_Implementation(ETouchIndex::Type FingerIndex, FVector2D Location)
{
    return false;
}

bool UPoolUserWidgetBase::TouchMoved_Custom_Implementation(ETouchIndex::Type FingerIndex, FVector2D Location)
{
    return false;
}


bool UPoolUserWidgetBase::TouchEnded_Custom_Implementation(ETouchIndex::Type FingerIndex, FVector2D Location)
{
    return false;
}

float UPoolUserWidgetBase::GetCuePower(int num)
{
    cueInfo.ChangeCueNum(num);
    return cueInfo.GetPower();
}
float UPoolUserWidgetBase::GetCueEffect(int num)
{
    cueInfo.ChangeCueNum(num);
    return cueInfo.GetEffect();
}
float UPoolUserWidgetBase::GetCueTime(int num)
{
    cueInfo.ChangeCueNum(num);
    return cueInfo.GetTime();
}

