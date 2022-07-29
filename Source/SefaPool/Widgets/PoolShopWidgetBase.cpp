// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolShopWidgetBase.h"

void UPoolShopWidgetBase::ActivateCue(int num)
{
    int idx = cues.Find(num);
    if (idx >= 0 && idx < cues.Num())
    {
        ActivateCueByIndex(idx);
    }
}

void UPoolShopWidgetBase::SetCuesArray(TArray<int32> cues_)
{
    //cues = cues_;
    //Init();
}
