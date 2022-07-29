// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayManager.h"

#include "ReplaySaveGame.h"
#include "ReplayVisualization.h"
#include "SefaPool/8Ball/BallsGatherer/BallManagerBase.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Widgets/Replay/ReplayWidget.h"
#include "Kismet/GameplayStatics.h"


AReplayManager::AReplayManager()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);
	bAlwaysRelevant = true;
	bIsMatchSaved = false;
}

void AReplayManager::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(ObjectGatherer);
	
}


void AReplayManager::TrySavePlayerNickNames()
{
	APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
		if (GS->FirstPlayerPS && !GS->FirstPlayerPS->username.IsEmpty())
		{
			FirstPlayerNickName = GS->FirstPlayerPS->username;
			if (GS->SecondPlayerPS && !GS->SecondPlayerPS->username.IsEmpty())
			{
				SecondPlayerNickName = GS->SecondPlayerPS->username;
				GetWorld()->GetTimerManager().ClearTimer(NickName_Timer);
				UE_LOG(LogTemp,Error,TEXT("Replay manager, PLAYERS NICK NAMES SAVED"))
			}
		}
	}
}

void AReplayManager::AppendStageVisualization(FStageData StageDatas)
{
	FrameBallArray.Add(StageDatas);
	int32 CurrentStageEndIndex = 0;
	for (int32 i =0;i < FrameBallArray.Num();++i)
	{
		CurrentStageEndIndex += FrameBallArray[i].BallInfoArray.Num()-1;
	}
	StagesEnd.Add(CurrentStageEndIndex);
	FStageSoundInfo NewSoundInfo = FStageSoundInfo();
	NewSoundInfo.SoundInfos.Append(SoundInfos);
	StageSoundInfos.Add(NewSoundInfo);
	UE_LOG(LogTemp,Warning,TEXT("Add stage visualization datas %i, Current stage end index is %i"),FrameBallArray.Num(),CurrentStageEndIndex)
}

void AReplayManager::AppendSoundInfo(FSoundInfo SoundInfo)
{
	SoundInfos.Add(SoundInfo);
	// UE_LOG(LogTemp,Error,TEXT("Added sound, sound info num %i"),SoundInfos.Num())
}

void AReplayManager::InitializeGameMode(ERecordedGameMode GMType)
{
	GameMode = GMType;
	GetWorld()->GetTimerManager().SetTimer(NickName_Timer,this,&AReplayManager::TrySavePlayerNickNames,2.f,true);
	UE_LOG(LogTemp,Error,TEXT("GM initialized %i"),GMType)
	switch (GameMode)
	{
	case ERecordedGameMode::CircleOfBalls:
	case ERecordedGameMode::LineUp:
	case ERecordedGameMode::FBNL:
	case ERecordedGameMode::SpeedPool:
	case ERecordedGameMode::PocketAsManyUCan:
		
		break;
		default:
			break;
	}
}

void AReplayManager::OnGameEnd(TArray<uint8>& replayData)
{
	if (bIsMatchSaved) return;
	bIsMatchSaved = true;
	UReplaySaveGame* SGInstance = Cast<UReplaySaveGame>(UGameplayStatics::CreateSaveGameObject(UReplaySaveGame::StaticClass()));
	if (SGInstance)
	{
		int32 TotalFrameCount = 0;
		SGInstance->GameMode = (uint8)GameMode;
		SGInstance->StageSoundInfo.Append(StageSoundInfos);
		SGInstance->FirstPlayerNickName = FirstPlayerNickName;
		SGInstance->SecondPlayerNickName = SecondPlayerNickName;
		for (int32 i = 0;i<FrameBallArray.Num();++i) TotalFrameCount+=FrameBallArray[i].BallInfoArray.Num();
		//UE_LOG(LogTemp,Error,TEXT("Total saved %i Sounds"),SoundInfos.Num())
		SGInstance->FrameBallArray.Append(FrameBallArray);
		SGInstance->StagesEnd.Append(StagesEnd);
		/*for (auto i : FrameBallArray)
		{
			for (auto j : i.BallInfoArray)
			{
				UE_LOG(LogTemp,Error,TEXT("SAVDE GAME TIME %i"),j.game_time)
			}
		}*/
		UGameplayStatics::SaveGameToSlot(SGInstance,FString::FromInt(117),0);
        //UGameplayStatics::SaveGameToMemory(SGInstance, replayData);
	}
}

void AReplayManager::LoadMatchReplay()
{
    FString slotName = FString::FromInt(117);
    
    /*USefaPoolGameInstance* GI = Cast<USefaPoolGameInstance>(GetWorld()->GetGameInstance());
    if (GI)
    {
        slotName = GI->data->replays->replayFileToPlay;
    }
    else
    {
        return;
    }*/
    
    UReplaySaveGame* SGInstance = Cast<UReplaySaveGame>(UGameplayStatics::LoadGameFromSlot(slotName,0));
	if (SGInstance)
	{
		FrameBallArray.Append(SGInstance->FrameBallArray);
		
		GameMode = (ERecordedGameMode)SGInstance->GameMode;
		StagesEnd.Append(SGInstance->StagesEnd);
		StageSoundInfos.Append(SGInstance->StageSoundInfo);
		UE_LOG(LogTemp,Error,TEXT("Loaded %i Sound datas"),SoundInfos.Num())
		FirstPlayerNickName = SGInstance->FirstPlayerNickName;
		SecondPlayerNickName = SGInstance->SecondPlayerNickName;
		for (auto i : FrameBallArray)
		{
			for (auto j : i.BallInfoArray)
			{
				UE_LOG(LogTemp,Error,TEXT("LOAD GAME TIME %i"),j.game_time)
			}
		}
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
		if (PC) 
		{
			APoolHUD* HUD = Cast<APoolHUD>(PC->GetHUD());
			if (HUD)
			{
				UReplayWidget* ReplayWidget = Cast<UReplayWidget>(HUD->userBarWidget);
				if (ReplayWidget)
				{
					ReplayWidget->SetPlayersNickNames(FirstPlayerNickName,SecondPlayerNickName);
					
				}else UE_LOG(LogTemp,Error,TEXT("Replay widget not valid"))
			}else UE_LOG(LogTemp,Error,TEXT("Replay HUD not valid"))
		}else UE_LOG(LogTemp,Error,TEXT("replay Player controller not valid"))
			// for (uint8 i = 0; i< SGInstance->PlayersID.Num();++i)
		// {
		// 
		// 	PlayerID.Add(PlayerID[i]);
		// }
		UE_LOG(LogTemp,Error,TEXT("STAGE NUM %i"),SGInstance->StagesEnd.Num())
		/*Multicast_ShareReplayData(StagesEnd);*/
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Can't get replay datas, data not exist or damaged"))
	}
	// for (int32 i =0;i< FrameBallArray.Num();++i)
	// {
	// 	UE_LOG(LogTemp,Error,TEXT("%f"),FrameBallArray[i].BallInfoArray[FrameBallArray[i].BallInfoArray.Num()-1].time)
	// }
	
}

void AReplayManager::PrepareToPlayReplay(APoolPlayerControllerBase* PlayerPC)
{
	switch (GameMode)
	{
		case ERecordedGameMode::CircleOfBalls:
		case ERecordedGameMode::LineUp:
		case ERecordedGameMode::FBNL:
		case ERecordedGameMode::SpeedPool:
		case ERecordedGameMode::PocketAsManyUCan:
		case ERecordedGameMode::Pool8Ball:
			// PlayerPC->ClientSetBallManagerSettings();
		UE_LOG(LogTemp,Warning,TEXT("Ball seted for COF, LineUp,FBNL,SpeedPool,PAMC, or 8Ball GM, GM id is %i"),GameMode)
		break;

		case ERecordedGameMode::Snooker:
			ObjectGatherer->BallManager->FirstBallIndex = 33;
			ObjectGatherer->BallManager->BallsCountInGame = 22;
			// PlayerPC->ClientSetBallManagerSettings(33,22);
		UE_LOG(LogTemp,Warning,TEXT("Ball seted for snooker GM"))
		break;
		case ERecordedGameMode::EnglishPool:
			ObjectGatherer->BallManager->FirstBallIndex = 16;
			ObjectGatherer->BallManager->BallsCountInGame = 16;
		break;
		default:
			break;
	}

	ObjectGatherer->BallManager->SetBallsForGM();
    // float lastTime = 0.0f;
	
	// for (int32 i =0; i < FrameBallArray.Num();++i)
	// {
	// 	for (int32 j =0;j < FrameBallArray[i].BallInfoArray.Num()-1;++j)
	// 	{
	// 		auto ballInfo = FrameBallArray[i].BallInfoArray[j];
	// 		ballInfo.time += lastTime;
	// 		ObjectGatherer->ballinfo->BallsInfoArray.Add(ballInfo);
	// 	}
	// 	lastTime += FrameBallArray[i].BallInfoArray[FrameBallArray[i].BallInfoArray.Num()-1].time;
	//     UE_LOG(LogTemp,Warning,TEXT("lastTime = %f"), lastTime)
	// }
	// for (int i = 0; i < FrameBallArray.Num();++i)
	// {
	// 	ReplayFrameCount += FrameBallArray[i].BallInfoArray.Num() - 1;
	// }
	/*ObjectGatherer->ballvisualization->ReinitToNextStage();*/
	// ObjectGatherer->ballinfo->StartClientUploadReplay();
	// UE_LOG(LogTemp,Warning,TEXT("Loaded and initialized %i frames info ball info class"),FrameBallArray.Num())

}

void AReplayManager::PlayReplay()
{
    ObjectGatherer->ballvisualization->StartVisualize();
}

void AReplayManager::InitializePlayerData(FString FirstPlayer, FString SecondPlayer)
{
	if (!FirstPlayer.IsEmpty())
	{
		FirstPlayerNickName = FirstPlayer;
	}
	if (!SecondPlayer.IsEmpty())
	{
		SecondPlayerNickName = SecondPlayer;
	}
}

void AReplayManager::Multicast_ShareReplayData_Implementation(const TArray<int32>& StagesEndArray)
{
	if (HasAuthority()) return;
	StagesEnd.Append(StagesEndArray);
	for (int32 i =0;i < StagesEnd.Num();++i)
	{
		UE_LOG(LogTemp,Error,TEXT("Stage %i last index is %i"),i+1,StagesEnd[i])
	}
	ObjectGatherer->ballinfo->SetIsReplayMode(true);
}

void AReplayManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// if (HasAuthority())
	// {
	// 	UE_LOG(LogTemp,Error,TEXT("ball array num on server side: %i"),ObjectGatherer->ballinfo->BallsInfoArray.Num())
	// }
	// if (GetLocalRole() < ROLE_Authority)
	// {
	// 	UE_LOG(LogTemp,Error,TEXT("ball array num on client side: %i"),ObjectGatherer->ballinfo->BallsInfoArray.Num())
	// }
}
//
// float AReplayManager::GetReplayProgress(int32 CurrentFrame)
// {
// 	// UE_LOG(LogTemp,Error,TEXT("Current frame %i, FrameCount %i"),CurrentFrame,ReplayFrameCount)
// 	return (float)CurrentFrame/(float)ReplayFrameCount;
// }
//
// int32 AReplayManager::GetFrameFromProgress(float Progress)
// {
// 	return FMath::RoundToInt(ReplayFrameCount*Progress);
// }



