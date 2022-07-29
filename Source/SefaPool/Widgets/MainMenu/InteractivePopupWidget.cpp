// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractivePopupWidget.h"

void UInteractivePopupWidget::RegisterCallbacks(
    const FPopupCallback& left_,
    const FPopupCallback& right_)
{
    left = left_;
    right = right_;
}

void UInteractivePopupWidget::CallLeft()
{
    left.Execute();
}

void UInteractivePopupWidget::CallRight()
{
    right.Execute();
}

