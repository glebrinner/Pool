// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolJoystickWidget.h"

#include "../Shared/PoolPlayerControllerBase.h"



UPoolJoystickWidget::UPoolJoystickWidget() :
    mode(JOYSTICK_MODE_DISABLED)
{
    
}

void UPoolJoystickWidget::RegisterJoystickMovement(FVector2D newPos, bool justStarted)
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    switch (mode)
    {
        case JOYSTICK_MODE_AIMING:
            pc->SetAimDisplacement(newPos);
            if (justStarted)
                pc->EffectMovementHasBegun();
            break;
        case JOYSTICK_MODE_3D:
            pc->Adjust3DCam(newPos);
            break;
        /*case JOYSTICK_MODE_BALL_IN_HAND:
            pc->BallInHandMoveBallIncremental(newPos);
            break;*/
        default:
            break;
    }
}

void UPoolJoystickWidget::RegisterJoystickRelease()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    switch (mode)
    {
        case JOYSTICK_MODE_AIMING:
            pc->EffectMovementHasEnded();
            break;
        case JOYSTICK_MODE_3D:
            pc->Adjust3DCam(FVector2D(0.0f));
            break;
        /*case JOYSTICK_MODE_BALL_IN_HAND:
            break;*/
        default:
            break;
    }
}


FVector2D UPoolJoystickWidget::GetRelaxedJoystickPosition()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return FVector2D(0.0f, 0.0f);
    if (mode == JOYSTICK_MODE_AIMING)
        return pc->GetAimDisplacement();
    return FVector2D(0.0f, 0.0f);
}

void UPoolJoystickWidget::ChangeMode(EJoystickModes mode_)
{
    mode = mode_;
    ChangeModeBP();
}

void UPoolJoystickWidget::ChangeModeBP_Implementation()
{}


void UPoolJoystickWidget::StartAngleChange()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->StartAngleChange();
}

void UPoolJoystickWidget::EndAngleChange()
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(GetOwningPlayer());
    if (!pc) return;
    pc->EndAngleChange();
}