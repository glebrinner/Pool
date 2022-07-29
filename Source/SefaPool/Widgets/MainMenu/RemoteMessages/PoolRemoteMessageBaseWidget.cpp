
#include "PoolRemoteMessageBaseWidget.h"
#include "PoolRemoteMessagesWidget.h"


void UPoolRemoteMessageBaseWidget::Parse(cool_json_t json)
{
    
}

void UPoolRemoteMessageBaseWidget::Dismiss()
{
    if (!parent) return;
    parent->gi->data->messages->MessageDismissed(id);
    parent->Update();
}
