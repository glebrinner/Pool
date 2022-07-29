// Fill out your copyright notice in the Description page of Project Settings.


#include "BallInfo.h"

#include "Pool8BallGameMode.h"
#include "Net/UnrealNetwork.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "Pool8BallPlayerController.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"

// Sets default values
ABallInfo::ABallInfo()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);
	bAlwaysRelevant = true;
    bIsReplayMode = false;
}

// Called when the game starts or when spawned
void ABallInfo::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(objects)
}

void ABallInfo::StartClientUpload()
{
    bShouldGetInfo = true;
    ClearClientSideData();
    GetWorldTimerManager().SetTimer(sendToClientsTimer,
                                    this,
                                    &ABallInfo::SendToClient_FromServer,
                                    0.5f,
                                    true,
                                    1.0f);
}

void ABallInfo::ClearClientSideData_Implementation()
{
    if (HasAuthority()) return;
    singlePosUpdates = 0;
}

void ABallInfo::StopClientUpload()
{
    bShouldGetInfo = false;
    GetWorldTimerManager().ClearTimer(sendToClientsTimer);
    SendToClient_FromServer();
    FStageData StageData;
    APool8BallGameMode* GM = Cast<APool8BallGameMode>(GetWorld()->GetAuthGameMode());
    if (GM)
    {
        StageData.ActivePlayerNum = GM->CurrentActivePlayerNum;
        UE_LOG(LogTemp,Error,TEXT("Set current active player num %i"),StageData.ActivePlayerNum)
    }else UE_LOG(LogTemp,Error,TEXT("Can't set current active player num"))
    StageData.BallInfoArray.Append(BallsInfoArray);
    UE_LOG(LogTemp,Warning,TEXT("Appended %i frames to stage data "),StageData.BallInfoArray.Num())
    objects->ReplayManager->AppendStageVisualization(StageData);
    ClearBallsInfoArray();
}

void ABallInfo::StopReplayUpload()
{
    GetWorldTimerManager().ClearTimer(sendToClientsTimer);
    UE_LOG(LogTemp,Warning,TEXT("Timer invalidate"))
}

void ABallInfo::StartClientUploadReplay()
{
    ClearClientSideData();
    GetWorldTimerManager().SetTimer(sendToClientsTimer,
                                    this,
                                    &ABallInfo::SendToClient_FromServer,
                                    0.2f,
                                    true,
                                    0.75f);
}

void ABallInfo::SendToClient_FromServer()
{
    SendToClient(GetNewDataPack(), GetNewSoundPack());
}

void ABallInfo::SendToClient_Implementation(const TArray<FBallsInfo>& ballsInfo, const TArray<FSoundInfo>& soundsInfo)
{
    if (HasAuthority() )
    {
        if (bIsReplayMode)
        {
            // UE_LOG(LogTemp,Error,TEXT("REPLAY MODE"))
            if (ballsInfo.Num() == 0)
            {
                StopReplayUpload();
                // UE_LOG(LogTemp,Warning,TEXT("Stop upload data"))
            }
        }
        return;
    }
    UE_LOG(LogTemp,Error,TEXT("Get visualization data from %s"),*GetName())
    BallsInfoArray.Append(ballsInfo);
    SoundsInfoArray.Append(soundsInfo);
   
   
    // UE_LOG(LogTemp,Error,TEXT("Ballinfo array updated! new count %i"),BallsInfoArray.Num())
    objects->ballvisualization->StartVisualize();
}

TArray<FBallsInfo> ABallInfo::GetNewDataPack()
{
	TArray<FBallsInfo> PartOfBallsInfo;
    int prev = Iterator;
    int32 BunchSize = 0;
    // UE_LOG(LogTemp,Error,TEXT("Send info to client ballinfo array size %i"),BallsInfoArray.Num())
    while (BallsInfoArray.IsValidIndex(Iterator))
    {
        PartOfBallsInfo.Add(BallsInfoArray[Iterator]);
        Iterator++;
        BunchSize++;
        if (BunchSize > 150)
        {
            return PartOfBallsInfo;
        }
    }
	return PartOfBallsInfo;
}

TArray<FSoundInfo> ABallInfo::GetNewSoundPack()
{
    TArray<FSoundInfo> PartOfSoundsInfo;
    int prev = IteratorSounds;
    
    while (SoundsInfoArray.IsValidIndex(IteratorSounds))
    {
        PartOfSoundsInfo.Add(SoundsInfoArray[IteratorSounds]);
        IteratorSounds++;
    }
    return PartOfSoundsInfo;
}

void ABallInfo::ClearBallsInfoArray()
{
	BallsInfoArray.Empty();
    Iterator = 0;
    SoundsInfoArray.Empty();
    IteratorSounds = 0;
}

void ABallInfo::UpdateBallPosOnClients()
{
    FBallsInfo pos = {};
    for(int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
    {
        pos.BallPos[i] = objects->physicsEngine->GetBallLocation(i);
        pos.BallRotation[i] = objects->physicsEngine->GetBallRotation(i);
    }
    SingleBallPosUpdate(pos);
}

void ABallInfo::UpdateBallPosOnSpecificClient(APool8BallPlayerController* pc)
{
    if (!pc) return;
    FBallsInfo pos = {};
    for(int i = 0; i < objects->physicsEngine->BallCountInGame; i++)
    {
        pos.BallPos[i] = objects->physicsEngine->GetBallLocation(i);
        pos.BallRotation[i] = objects->physicsEngine->GetBallRotation(i);
    }
    pc->UpdateClientBallPos(pos);
}

void ABallInfo::SingleBallPosUpdate_Implementation(const FBallsInfo& pos)
{
    singlePosUpdates++;
    if (HasAuthority()) return;
    objects->ballvisualization->VisualizeSnapshot(pos);
}
