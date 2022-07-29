
// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPoolGameInstance.h"
#include "SefaPool/Data/SefaPoolData.h"
#include "SefaPool/Widgets/MainMenu/RemoteMessages/PoolRemoteMessagesWidget.h"
#include "SharedDefs.h"


void from_json(const cool_json_t& json, EDifficultyLevels& level)
{
    int value = json.get<int>();
    if (value <= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_NONE) ||
        value >= static_cast<int>(EDifficultyLevels::DIFFICULTY_LEVEL_6))
        level = EDifficultyLevels::DIFFICULTY_LEVEL_6;
    else
        level = static_cast<EDifficultyLevels> (value);
}

void to_json(cool_json_t& json, const EDifficultyLevels& level)
{
    json = cool_json_t(static_cast<int> (level));
}


FItemDurability::FItemDurability()
{
    cue_durabilities.SetNum(PREDEFINED_CUES_N + 1);
    cue_usages.SetNum(PREDEFINED_CUES_N + 1);
    cue_recharges.SetNum(PREDEFINED_CUES_N + 1);
    chalk_durabilities.SetNum(PREDEFINED_CHALKS_N + 1);
    chalk_usages.SetNum(PREDEFINED_CHALKS_N + 1);
    chalk_recharges.SetNum(PREDEFINED_CHALKS_N + 1);
    for (int i = 1; i < PREDEFINED_CUES_N + 1; ++i)
    {
        cue_durabilities[i] = -1;
        cue_usages[i] = 0;
        cue_recharges[i] = 0;
    }
    for (int i = 1; i < PREDEFINED_CHALKS_N + 1; ++i)
    {
        chalk_durabilities[i] = -1;
        chalk_usages[i] = 0;
        chalk_recharges[i] = 0;
    }
};

bool FItemDurability::ChangeConsumableDurability(EConsumableType type, int num, int value)
{
    if (type == EConsumableType::CUE)
    {
        if (num > PREDEFINED_CUES_N || num < 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cue number %d doesn't lie in [1, %u]."), num, PREDEFINED_CUES_N);
            return false;
        }
        if (cue_durabilities[num] + value < 0)
            return false;

        cue_durabilities[num] += value;
        UE_LOG(LogTemp, Warning, TEXT("Cue durability with num %d was changed: %d."), num, cue_durabilities[num]);
        if (value < 0)
        {
            cue_usages[num] -= value;
            UE_LOG(LogTemp, Warning, TEXT("Cue usage with num %d was changed: %d."), num, cue_usages[num]);
        }

        return true;
    }
    else
    if (type == EConsumableType::CHALK)
    {
        if (num > PREDEFINED_CHALKS_N || num < 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Chalk number %d doesn't lie in [1, %u]."), num, PREDEFINED_CHALKS_N);
            return false;
        }
        if (chalk_durabilities[num] + value < 0)
            return false;

        chalk_durabilities[num] += value;
        UE_LOG(LogTemp, Warning, TEXT("Chalk durability with num %d was changed: %d."), num, chalk_durabilities[num]);
        
        if (value < 0)
        {
            chalk_usages[num] -= value;
            UE_LOG(LogTemp, Warning, TEXT("Chalk usage with num %d was changed: %d."), num, chalk_usages[num]);
        }

        return true;
    }
    return false;
}

bool FItemDurability::RechargeConsumable(EConsumableType type, int num)
{
    if (type == EConsumableType::CUE)
    {
        cue_recharges[num] += 1;
        return true;
    }
    else
    {
        chalk_recharges[num] += 1;
        return true;
    }
}


void USefaPoolGameInstance::Init()
{
    Super::Init();
    // UPoolGameSettings* test = NewObject<UPoolGameSettings>();
    // test->SetHandOrientation(EHandOrientation::RightHanded);
    // UE_LOG(LogTemp,Error,TEXT("Config FILE VALUE %f"),test->GetFocusCameraHorizontalSensitivity());
    // GEngine->AddOnScreenDebugMessage(0,10.f,FColor::Yellow,"Config File Value " + FString::SanitizeFloat(test->GetFocusCameraHorizontalSensitivity()));
    // test->SaveConfig();
    //UE_LOG(LogTemp,Error,TEXT("CONFIG SAVED"))
    
    widgetManager = NewObject<UWidgetManager> ();
    InitWidgetManager();
    
    orientationManager = NewObject<UOrientationManager> ();
    
    data = NewObject<USefaPoolData> ();
    data->Init(this);
    
}

void USefaPoolGameInstance::SetItemDurability(const FItemDurability &item_durability)
{
    item_durability_ = item_durability;
    PRINTF((TEXT("Item durability was set.")), Red);
    int i = 0;
    //for (const auto& cue : item_durability_.cue_durability)
    //{
    //    PRINTF((TEXT("cue durability %d: %d"), i++, cue), Red);
    //}
    //PRINTF((TEXT("chulk durability %d"), item_durability.chalk_durability), Red);
}

int FItemDurability::GetDurability(EConsumableType type, int num)
{
    if (type == EConsumableType::CUE)
    {
        if (num > PREDEFINED_CUES_N || num < 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cue number %d doesn't lie in [1, %u]."), num, PREDEFINED_CUES_N);
            return -1;
        }
        return cue_durabilities[num];
    }
    else
    {
        if (num > PREDEFINED_CHALKS_N || num < 1)
        {
            UE_LOG(LogTemp, Warning, TEXT("Chalk number %d doesn't lie in [1, %u]."), num, PREDEFINED_CUES_N);
            return -1;
        }
        return chalk_durabilities[num];
    }
}


void USefaPoolGameInstance::RequestEntyDataForMainMenu(const FResponseDelegat& succesful,
                                                          const FResponseDelegat& unsuccesful)
{
    
    data->shop->Pull();
    data->user->Pull();
    data->inventory->Pull();
    data->messages->PullWithCallbacks([this](USefaPoolDataBlock* messages_) {
        USefaPoolMessages* messages = dynamic_cast<USefaPoolMessages*> (messages_);
        if (!messages_) return;
        auto messagesWidget = dynamic_cast<UPoolRemoteMessagesWidget*> (widgetManager->GetWidget("RemoteMessages"));
        if (messagesWidget)
        {
            messagesWidget->Update();
        }
        ShowNewMessagePopup(messages->GetLatestMessage());
    }, nullptr);
    data->friends->Pull();
    data->settings->Pull();
    data->replays->Pull();
    data->stats->Pull();
    //succesful.Execute();
    
    auto callback = [this, succesful, unsuccesful](cool_json_t json) -> void {
        PRINTF((TEXT("RequestEntyDataForMainMenu successful callback")), Red);
        UE_LOG(LogTemp, Warning, TEXT("%s\n"), *FString(json.dump(3).c_str()));
        PRINTF((TEXT("After print json.")), Red);
        bool is_status_ok = false;
        bool is_all_data_recieved = true;
        cool_json_t json_response(json);
        if (!json_response.is_null())
        {
            // CHECKS ONLY
            // shop/get_bought_items
            /*is_all_data_recieved &= check_status(json_response["shop"]["get_bought_items"], FString("shop/get_bought_items"));
            UE_LOG(LogTemp, Warning, TEXT("USefaPoolGameInstance::RequestEntyDataForMainMenu is_all_data_recieved after shop/get_bought_items %d"), is_all_data_recieved);
            
            //user/get_player_info
            is_all_data_recieved &= check_status(json_response["user"]["get_player_info"],
                                        FString("user/get_player_info"));
            UE_LOG(LogTemp, Warning, TEXT("USefaPoolGameInstance::RequestEntyDataForMainMenu is_all_data_recieved after user/get_player_info %d"), is_all_data_recieved);
            //Reading user/get_user_info
            is_all_data_recieved &= check_status(json_response["user"]["get_user_info"],
                                        FString("user/get_user_info"));
            UE_LOG(LogTemp, Warning, TEXT("USefaPoolGameInstance::RequestEntyDataForMainMenu is_all_data_recieved after user/get_user_info %d"), is_all_data_recieved);
            //if (is_all_data_recieved)
            //    json_response.get_to(entry_data);
            */
            
            if (is_all_data_recieved)
                succesful.Execute();
            else {
                UE_LOG(LogTemp, Warning, TEXT("USefaPoolGameInstance::RequestEntyDataForMainMenu Not all data received"));
                unsuccesful.Execute();
            }
        }
        else {
            UE_LOG(LogTemp, Warning, TEXT("USefaPoolGameInstance::RequestEntyDataForMainMenu json null response"));
            unsuccesful.Execute();
        }
    };
    USefaPoolGameInstance* gi = dynamic_cast<USefaPoolGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi)
    {
        gi->MakeRequestWithFail("api", "get_intro",
                                cool_json_t(),
                                callback,
                                [unsuccesful] () {unsuccesful.Execute();});
    }
}

void USefaPoolGameInstance::SwitchToOrientation(E_ScreenOrientation orientation)
{
    APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (this && orientationManager)
        orientationManager->SwitchToOrientation(orientation, pc);
}


void USefaPoolGameInstance::PlayingOffline()
{
    Super::PlayingOffline();

}