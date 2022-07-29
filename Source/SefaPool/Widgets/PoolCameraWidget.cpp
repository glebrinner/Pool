// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolCameraWidget.h"
#include "../Shared/PoolPlayerControllerBase.h"
#include "Kismet/GameplayStatics.h"

void UPoolCameraWidget::InitiateSwitch2D()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->Switch2DCamera();
}

void UPoolCameraWidget::InitiateSwitch3D()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->Switch3DCamera();
}

void UPoolCameraWidget::Stop3D()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->Stop3DCamera();
}

void UPoolCameraWidget::RegisterJoystickMovement(FVector2D newPos)
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->Adjust3DCam(newPos);
}


void UPoolCameraWidget::Activate2DButton_Implementation()
{
    
}

void UPoolCameraWidget::Activate3DButton_Implementation()
{
    
}

void UPoolCameraWidget::DeactivateButtons_Implementation()
{
    
}
