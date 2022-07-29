#include "PoolServerMatchSelectorGameMode.h"

APoolServerMatchSelectorGameMode::APoolServerMatchSelectorGameMode()
{
#if UE_SERVER
    AgonesSDK = CreateDefaultSubobject<UAgonesComponent>(TEXT("AgonesSDK"));
    FNetworkVersion::IsNetworkCompatibleOverride.BindLambda([](uint32 LocalNetworkVersion, uint32 RemoteNetworkVersion)
    {
        return true;
    });
    

#endif
    
    //start tcp server
}


void APoolServerMatchSelectorGameMode::BeginPlay()
{
    Super::BeginPlay();
    /*TcpSocket = dynamic_cast<ATcpComunicationActor*>((GetWorld()->SpawnActor<ATcpComunicationActor>(FVector(0, 0, 0), FRotator(0, 0, 0))));
    TcpSocket->set_handle_query([=](uint8* ReceivedData, uint32 Size) { TCPArrived(ReceivedData, Size); });
    TcpSocket->StartTCPReceiver(FString("MatchSelectorTcpSocket"), FString("0.0.0.0"), 2590);*/

    UE_LOG(LogTemp, Error, TEXT("Loading up server GM"));

    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));

    if (this && gi)
    {
        UE_LOG(LogTemp, Warning, TEXT("GI checked, all good"));
    }
    else
        UE_LOG(LogTemp, Warning, TEXT("Bad GI"));
    
    FGameServerResponse tmp = {};
    ReadyAgones(tmp);
    FScriptDelegate agones_reply_delegat;
    agones_reply_delegat.BindUFunction(this, "ReadyAgones");
    AgonesSDK->ConnectedDelegate.AddUnique(agones_reply_delegat);
}

void APoolServerMatchSelectorGameMode::ReadyAgones(const FGameServerResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocating game server"));
    FAgonesErrorDelegate error_del;
    error_del.BindUFunction(this, "AgonesError");
    FReadyDelegate success_del;
    success_del.BindUFunction(this, "AgonesSuccess");
    AgonesSDK->Ready(success_del, error_del);
}

void APoolServerMatchSelectorGameMode::AgonesSuccess(const FEmptyResponse& Response) {
    UE_LOG(LogTemp, Error, TEXT("Allocation was succesful"));
    
    FGameServerDelegate del;
    del.BindUFunction(this, "GameServerCallback");
    AgonesSDK->WatchGameServer(del);
    
}

void APoolServerMatchSelectorGameMode::AgonesError(const FAgonesError& Response) {
    UE_LOG(LogTemp, Error, TEXT("%s"), *Response.ErrorMessage);
}


/*
void APoolServerMatchSelectorGameMode::TCPArrived(uint8* ReceivedData, uint32 Size)
{

    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));

    if (this && AgonesSDK && gi)
    {
        memcpy(gi->tcp_received_key, ReceivedData, Size);
        if (gi->tcp_received_key == nullptr)
        {
            UE_LOG(LogTemp, Error, TEXT("TCP received key was the wrong size"));
        }
        FGameServerDelegate del;
        del.BindUFunction(this, "GameServerCallback");
        AgonesSDK->GameServer(del, {});
    }
}*/
void APoolServerMatchSelectorGameMode::GameServerCallback(const FGameServerResponse& gameServerInfo)
{
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    TRACEWARN("Received update on WatchGameServer")
    if (this && gi)
    {
        const TMap<FString, FString>& annotations = gameServerInfo.ObjectMeta.Annotations;
        if (!annotations.Contains("game_type"))
            return;
        gi->ParseAnnotations(annotations);
    }
}


 
