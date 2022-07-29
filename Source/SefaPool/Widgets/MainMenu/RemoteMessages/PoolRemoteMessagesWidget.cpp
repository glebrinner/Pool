

#include "PoolRemoteMessagesWidget.h"
#include "Blueprint/WidgetTree.h"

void UPoolRemoteMessagesWidget::Update()
{
    /*for (auto& w : messageWidgets)
    {
        if (w) w->RemoveFromParent();
    }
    messageWidgets.Empty();
    
    for (auto& message : messages)
    {
        UE_LOG(LogTemp, Warning,
               TEXT("UPoolRemoteMessagesWidget::Update: Generating widget for message: %s"), *FString(message.dump().c_str()));
        ERemoteMessageType messageType = ERemoteMessageType::NO_MESSAGE;
        message.get_to (messageType);
        if (messageType == ERemoteMessageType::NO_MESSAGE) continue;
        TSubclassOf<UPoolRemoteMessageBaseWidget> messageWidgetClass = messageWidgetClasses[messageType];
        if (!messageWidgetClass)
        {
            UE_LOG(LogTemp, Warning,
                   TEXT("UPoolRemoteMessagesWidget::Update: Class not found"));
            continue;
        }
        auto newMessageWidget = WidgetTree->ConstructWidget<UPoolRemoteMessageBaseWidget>(messageWidgetClass);
        messageWidgets.Add(newMessageWidget);
        newMessageWidget->parent = this;
        UE_LOG(LogTemp, Warning,
               TEXT("UPoolRemoteMessagesWidget::Update: calling parse"));
        newMessageWidget->Parse(message);
    }
    RelinkMessageWidgets();*/

    for (auto& w : messageWidgets)
    {
        if (w.widget) w.widget->RemoveFromParent();
    }
    messageWidgets.Empty();
    

    for (auto& message : gi->data->messages->messages)
    {
        FMessageWidgetInfo info = {
            nullptr, 
            message.Value.shortDescription, 
            message.Value.read,
            message.Key
        };
        messageWidgets.Add(info);
    }
    
    RelinkMessageWidgets();
}

void UPoolRemoteMessagesWidget::OpenSpecificMessageWidget(int id)
{
    if (!gi->data->messages->messages.Contains(id)) return;
    //if (gi->data->messages->messages.Find(id)->read) return;
    FRemoteMessageInfo& rmi = *gi->data->messages->messages.Find(id);
    cool_json_t messageJson = cool_json_t::parse(TCHAR_TO_UTF8(*rmi.messageJson), nullptr, false);
    ERemoteMessageType messageType = ERemoteMessageType::NO_MESSAGE;
    messageJson.get_to(messageType);
    TSubclassOf<UPoolRemoteMessageBaseWidget> messageWidgetClass = messageWidgetClasses[messageType];
    if (!messageWidgetClass)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UPoolRemoteMessagesWidget::Update: Class not found"));
        return;
    }
    auto specificMessageWidget = WidgetTree->ConstructWidget<UPoolRemoteMessageBaseWidget>(messageWidgetClass);

    specificMessageWidget->parent = this;
    specificMessageWidget->id = id;
    specificMessageWidget->Parse(messageJson);

    gi->data->messages->MessageRead(id);

    ShowSpecificMessageWidget(specificMessageWidget);

    Update();
}