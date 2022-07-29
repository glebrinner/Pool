// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolMessageDeliveryWidget.h"

#include "Messages_eng1.h"

void UPoolMessageDeliveryWidget::Init()
{
    FillMessageText_ENG(MessageText);
   
}

void UPoolMessageDeliveryWidget::EnqueueMessageForDisplay(E_MessageSeverity type, uint16 messageId)
{
    switch (type)
    {
        case E_MessageSeverity::IGNORED:
            break;
        case E_MessageSeverity::SECONDARY:
            secondaryMessages.Enqueue (messageId);
            break;
        case E_MessageSeverity::PRIMARY:
            primaryMessages.Enqueue (messageId);
            break;
        case E_MessageSeverity::CHAT:
            chatMessages.Enqueue (messageId);
            break;
    }
}

bool UPoolMessageDeliveryWidget::PrimaryEmpty()
{
    return primaryMessages.IsEmpty();
}

FString UPoolMessageDeliveryWidget::GetPrimaryMessage()
{
    if (primaryMessages.IsEmpty())
        return TEXT("");
    uint16 primId = 0;
    primaryMessages.Dequeue(primId);
    return MessageText[primId];
}

bool UPoolMessageDeliveryWidget::SecondaryEmpty()
{
    return secondaryMessages.IsEmpty();
}

FString UPoolMessageDeliveryWidget::GetSecondaryMessage()
{
    if (secondaryMessages.IsEmpty())
        return TEXT("");
    uint16 secId = 0;
    secondaryMessages.Dequeue(secId);
    return MessageText[secId];
}

bool UPoolMessageDeliveryWidget::ChatEmpty()
{
    return chatMessages.IsEmpty();
}

FString UPoolMessageDeliveryWidget::GetChatMessage()
{
    if (chatMessages.IsEmpty())
        return TEXT("");
    uint16 chatId = 0;
    chatMessages.Dequeue(chatId);
    return MessageText[chatId];
}
