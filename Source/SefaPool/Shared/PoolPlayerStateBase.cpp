// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolPlayerStateBase.h"
#include "Engine/ActorChannel.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PoolPlayerControllerBase.h"
#include "PoolHUD.h"
#include "ObjectGatherer.h"
#include "PoolGameStateBase.h"
#include "SefaPool/Server/PoolServerGameInstance.h"


APoolPlayerStateBase::APoolPlayerStateBase() :
    cueInfo (),
    item_durability_()
{
    ballInHand = false;
    bReplicates = true;
    NetUpdateFrequency = 1;
    username = "";
}

void APoolPlayerStateBase::Client_ForceUpdateTargetableBalls_Implementation()
{
    // ObjectGatherer->playerAimLine->ForceUpdateTargetableBalls();
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
    if (PC)
    {
        APoolPlayer* PP = Cast<APoolPlayer>(PC->GetPawn());
        if (PP)
        {
            PP->Readjust();
        }
    }
}

void APoolPlayerStateBase::BeginPlay()
{
    Super::BeginPlay();
    GATHER_OBJECT_GATHERER(ObjectGatherer);
}
    
bool APoolPlayerStateBase::CanSelectCue(int num)
{
    if (num < 1 || num > PREDEFINED_CUES_N)
        return false;
    return item_durability_.cue_durabilities[num] >= 0;
}


bool APoolPlayerStateBase::CanSelectChalk(int num)
{
    if (num < 1 || num > PREDEFINED_CHALKS_N)
        return false;

    return item_durability_.chalk_durabilities[num] >= 0;
}

bool APoolPlayerStateBase::SelectCue(int num)
{
    if (!CanSelectCue(num))
    {
        cueInfo.ChangeCueNum(1);
        return false;
    }
    cueInfo.ChangeCueNum(num);
    UE_LOG(LogTemp, Warning, TEXT("Selected cue was %d"), num);
    return true;
}

bool APoolPlayerStateBase::SelectChalk(int num)
{
    if (!CanSelectChalk(num))
    {
        cueInfo.ChangeChalkNum(1);
        return false;
    }
    cueInfo.ChangeChalkNum(num);
    UE_LOG(LogTemp, Warning, TEXT("Selected chalk was %d"), num);
    return true;
}
void APoolPlayerStateBase::UpdateClient()
{
    if (!HasAuthority()) return;
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    UBaseBackendInfo* backend_data = gi ? dynamic_cast<UBaseBackendInfo*>(gi->backend_data) : nullptr;
    FBackendPlayerInfo* player_info = backend_data ? backend_data->GetPlayerInfo(user_id) : nullptr;
    
    if (player_info)
        item_durability_ = player_info->durability;
    
    if (GetNetDriver())
    {
        for (UNetConnection* Connection : GetNetDriver()->ClientConnections)
        {
            //if (Connection->OwningActor != GetOwner()) continue;
            UActorChannel** Channel = Connection->FindActorChannel(this);
            if (Channel != nullptr && *Channel != nullptr)
                (*Channel)->ReplicateActor();
        }
    }
}


void APoolPlayerStateBase::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME( APoolPlayerStateBase, cueInfo );
    DOREPLIFETIME( APoolPlayerStateBase, ballInHand );
    DOREPLIFETIME( APoolPlayerStateBase, item_durability_);
    DOREPLIFETIME( APoolPlayerStateBase, username);
    DOREPLIFETIME( APoolPlayerStateBase, user_id);
}

void APoolPlayerStateBase::InitClient_Implementation(const TArray<int32>& cue_durabilities,
                                                     const TArray<int32>& chalk_durabilities,
                                                     int current_cue,
                                                     int current_chalk,
                                                     EDifficultyLevels difficulty)
{
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

    if (HasAuthority() || (pc && pc->PlayerState == this && pc->IsLocalPlayerController()))
    {
        item_durability_.cue_durabilities = cue_durabilities;
        item_durability_.chalk_durabilities = chalk_durabilities;
        SelectCue(current_cue);
        SelectChalk(current_chalk);
        
        if (pc && pc->PlayerState == this && pc->IsLocalPlayerController())
        {
            auto* game_instance = Cast<USefaPoolGameInstance>(GetGameInstance());
            if (game_instance)
            {
                game_instance->SetItemDurability(item_durability_);
            }
            pc->GetCueComponent()->SelectCue(current_cue);
            pc->InitShop();
            DiffLevel = difficulty;
            pc->objects->playerAimLine->ChangeLevel(DiffLevel);
        }
    }
}


void APoolPlayerStateBase::ConsumablesReady()
{
    //APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    //if (pc && pc->PlayerState == this && pc->IsLocalPlayerController())
    //{
    //    pc->UploadConsumablesToStore(item_durability_.cue_durabilities, item_durability_.chalk_durabilities);
    //}
    
}


FItemDurability& APoolPlayerStateBase::GetItemDurability()
{
    return item_durability_;
}

int APoolPlayerStateBase::GetCurrentCue()
{
    return cueInfo.cueNum;
}

int APoolPlayerStateBase::GetCurrentChalk()
{
    return cueInfo.chalkNum;
}

void APoolPlayerStateBase::TransferItemDurabilityToGameInstance_Implementation()
{
    auto* game_instance = Cast<USefaPoolGameInstance>(GetGameInstance());
    if (game_instance)
    {
        //PRINTF((TEXT("TransferItemDurabilityToGameInstance_Implementation")), Red);
        //for (auto pair : item_durability.cue_durabilities)
        //{
        //	PRINTF((TEXT("%d"), cue), Red);
        //}
        game_instance->SetItemDurability(item_durability_);
    }
    else
    {
        PRINTF((TEXT("Game Instance is null_ptr")), Red);
    }
}

bool APoolPlayerStateBase::CanTargetThisBall(int n)
{
    bool bIsFound = false;
    for (int32 i =0;i<TargetableBalls.Num();++i)
    {
        if (TargetableBalls[i] == n)
        {
            bIsFound = true;
            break;
        }
    }
    return bIsFound;
}

void APoolPlayerStateBase::ClientSetTargetableBalls_Implementation(const TArray<uint8>& ArrayOfTargetableBall)
{
    TargetableBalls.Reset();
    TargetableBalls.Append(ArrayOfTargetableBall);
    UE_LOG(LogTemp,Error,TEXT("Set targetable balls get %i balls seted %i balls"),ArrayOfTargetableBall.Num(),TargetableBalls.Num())
}


void APoolPlayerStateBase::RegisterMessage(E_MessageSeverity severity, uint16 messageId)
{
    if (!HasAuthority()) return;
    FMessageInfo info = {severity, messageId};
    activeMessages.Add(info);
    UE_LOG(LogTemp,Error,TEXT("registered message from id %i"),messageId)
}

void APoolPlayerStateBase::SendImmediateMessage(E_MessageSeverity severity, uint16 messageId)
{
    if (!HasAuthority()) return;
    TArray<FMessageInfo> toSend;
    FMessageInfo info = {severity, messageId};
    toSend.Add(info);
    Client_DeployMessages(toSend);
}

void APoolPlayerStateBase::ClearMessages()
{
    activeMessages.Empty();
}

void APoolPlayerStateBase::Server_DeployMessages()
{
    if (!HasAuthority()) return;
    TArray<FMessageInfo> toSend (activeMessages);
    activeMessages.Empty();
    Client_DeployMessages(toSend);
}

void APoolPlayerStateBase::Client_DeployMessages_Implementation(const TArray<FMessageInfo>& mess)
{
    UE_LOG(LogTemp,Warning,TEXT("Start deploy message on client side, message count %i"),mess.Num())
    APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
    if (pc && pc->PlayerState == this && pc->IsLocalPlayerController())
    {
        for (auto message : mess)
        {
            pc->DisplayMessage(message.severity, message.messageId);
            UE_LOG(LogTemp,Error,TEXT("message displayer on client side"))
        }
    }
}


