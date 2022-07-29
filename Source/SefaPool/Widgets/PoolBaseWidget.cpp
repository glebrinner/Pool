// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolBaseWidget.h"

void UPoolBaseWidget::OnLevelRemovedFromWorld(ULevel * InLevel,
                                              UWorld * InWorld)
{
    if (transient)
        RemoveFromParent();
    else
    if (hideOnLevelChange)
        SetVisibility(ESlateVisibility::Collapsed);
}

void UPoolBaseWidget::Hide()
{
    HidePrep();
    SetVisibility(ESlateVisibility::Collapsed);
}

void UPoolBaseWidget::Show()
{
    ShowPrep();
    SetVisibility(ESlateVisibility::Visible);
}
