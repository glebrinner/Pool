// Fill out your copyright notice in the Description page of Project Settings.


#include "BallsVisualization.h"
#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"
#include "SefaPool/Widgets/Replay/ReplayWidget.h"

// Sets default values
ABallsVisualization::ABallsVisualization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bShouldVisualize = false;
	iterator = 0;
	SetReplicates(true);
	bAlwaysRelevant = true;
	bAutoPlayReplay = true;
    shotBeginTime = 0.0f;
	CurrentStage = 0;
}

// Called when the game starts or when spawned
void ABallsVisualization::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(objects)
	BallInfo = objects->ballinfo;
	shotBeginTime = 0.0f;
}

// Called every frame
void ABallsVisualization::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShouldVisualize)
	{
		// UE_LOG(LogTemp,Error,TEXT("%i"),objects->ballinfo->BallsInfoArray.Num())
		// UE_LOG(LogTemp,Error,TEXT("Start visualize %i"),objects->ballinfo->BallsInfoArray.Num())
		// if (BallInfo->IsReplayMode())
		// {
		// 	int32 Stage = GetCurrentStage(iterator);
		// 	if (Stage != INDEX_NONE)
		// 	{
		// 		if (Stage != CurrentStage && Stage > CurrentStage)
		// 		{
		// 			UE_LOG(LogTemp,Error,TEXT("STAGE CHANGED from %i to %i"),CurrentStage,Stage)
		// 			CurrentStage = Stage;
		// 			// shotBeginTime = GetGameTimeSinceCreation();
		//
		//    //                int times = 2;
		// 			// iterator = objects->ReplayManager->GetStagesEnd()[CurrentStage];
		// 			// for (int i = -times; i <= times; i++)
		// 			//     UE_LOG(LogTemp,Error,TEXT("time[current + (%d)] = %f"), i, BallInfo->BallsInfoArray[iterator+i].time)
		// 		}
		// 	}
		// }
		float currentTime = (GetGameTimeSinceCreation() - shotBeginTime);
		// if (BallInfo->BallsInfoArray.IsValidIndex(iterator)) UE_LOG(LogTemp,Error,TEXT("current time %f, serverTime %f"), currentTime, BallInfo->BallsInfoArray[iterator].time)
		//
		while (!soundVizHasEnded &&
               BallInfo->SoundsInfoArray.IsValidIndex(soundIterator) &&
		       BallInfo->SoundsInfoArray[soundIterator].time - ReplayMovableZero < currentTime)
		{
			UE_LOG(LogTemp,Error,TEXT("Sound played iter %i, rec time %f, curr time %f"),soundIterator, BallInfo->SoundsInfoArray[soundIterator].time, currentTime)
		    objects->sound->EmitSound(BallInfo->SoundsInfoArray[soundIterator].sound,
		                              BallInfo->SoundsInfoArray[soundIterator].pos,
		                              BallInfo->SoundsInfoArray[soundIterator].power);
            if (BallInfo->SoundsInfoArray.IsValidIndex(soundIterator+1) &&
                BallInfo->SoundsInfoArray[soundIterator+1].time <
                BallInfo->SoundsInfoArray[soundIterator].time)
                soundVizHasEnded = true;
		    soundIterator++;
		}
        
		// UE_LOG(LogTemp,Error,TEXT("Iterator value %i"),iterator)
		while (BallInfo->BallsInfoArray.IsValidIndex(iterator) &&
			BallInfo->BallsInfoArray[iterator].time - ReplayMovableZero < currentTime)
		{
			// UE_LOG(LogTemp,Error,TEXT("skipping frame %d, serverTime %f, clientTime %f, stage %d"), iterator, BallInfo->BallsInfoArray[iterator].time, currentTime, GetCurrentStage(iterator))
			iterator++;
			// todo конец хода, вызвать некст ход
			// if (BallInfo->IsReplayMode())
			//      	{
			//        int32 Stage = GetCurrentStage(iterator);
			//       	if (Stage != INDEX_NONE)
			//       	{
			//       		if (Stage != CurrentStage && Stage > CurrentStage)
			//       		{
			//       			UE_LOG(LogTemp,Error,TEXT("ITERATOR MOVE TO NEXT STAGE"))
			//       			return;
			//       		}
			//       	}
			//       }
		}
		if (iterator == 0 && (BallInfo->BallsInfoArray.IsValidIndex(iterator)))
		{
			VisualizeSnapshotNow(BallInfo->BallsInfoArray[0]);
			// UE_LOG(LogTemp,Warning,TEXT("Visualize snapshot now"))
		}
		else if (BallInfo->BallsInfoArray.IsValidIndex(iterator))
		{
			// UE_LOG(LogTemp,Warning,TEXT("Ballinfoarray has valid iterator"))
			iterator--;
			float dt_from_server = BallInfo->BallsInfoArray[iterator + 1].time -
				BallInfo->BallsInfoArray[iterator].time;
			float dt_on_client = BallInfo->BallsInfoArray[iterator + 1].time - ReplayMovableZero - currentTime;
			float coeff = (dt_from_server - dt_on_client) / dt_from_server;
            //UE_LOG (LogTemp, Warning, TEXT("COEFF : %f"), iterator + coeff)
			coeff = 0.f;
			for (int i=0; i < BallCountInGame; i++)
			{
				FVector targetPos = BallInfo->BallsInfoArray[iterator].BallPos[i]     * coeff;
				targetPos +=        BallInfo->BallsInfoArray[iterator + 1].BallPos[i] * (1.0f - coeff);
				FQuat targetQuat =  FQuat::Slerp(BallInfo->BallsInfoArray[iterator].BallRotation[i].Quaternion(),
										BallInfo->BallsInfoArray[iterator + 1].BallRotation[i].Quaternion(),
										coeff);
				objects->physicsEngine->MoveBallAndStop(i, targetPos, targetQuat.Rotator());
			}
		}
		else if (BallInfo->BallsInfoArray.IsValidIndex(iterator - 1) && !BallInfo->BallsInfoArray.IsValidIndex(iterator))
		{
			VisualizeSnapshotNow(BallInfo->BallsInfoArray[iterator - 1]);
			iterator++;
			// UE_LOG(LogTemp,Warning,TEXT("Ballinfo array has valid iterator -1"))
		}
		// // else
		// if (BallInfo->BallsInfoArray.IsValidIndex(iterator))
		// {
		// 	VisualizeSnapshotNow(BallInfo->BallsInfoArray[iterator]);
		// 	iterator++;
		// }
		else
		{
		UE_LOG(LogTemp,Error,TEXT("Visualize else"))
			BallInfo->BallsInfoArray.Reset();
			soundIterator = 0;
			iterator = 0;
			BallInfo->SoundsInfoArray.Reset();
			bShouldVisualize = false;
			if (BallInfo->IsReplayMode() && bAutoPlayReplay)
			{
				ReinitToNextStage();
				
			}
            if (!HasAuthority())
            {
                if (objects && objects->hud && objects->hud->userBarWidget)
                {
                    objects->hud->userBarWidget->BallsStopped();
                }
            }
		}
		if (BallInfo->IsReplayMode())
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
			if (PC)
			{
				APoolHUD* HUD = Cast<APoolHUD>(PC->GetHUD());
				if (HUD)
				{
					UReplayWidget* ReplayWidget = Cast<UReplayWidget>(HUD->userBarWidget);
					if (ReplayWidget && BallInfo->BallsInfoArray.IsValidIndex(iterator))
					{
						ReplayWidget->SetGameTime(BallInfo->BallsInfoArray[iterator].game_time);
						UE_LOG(LogTemp,Error,TEXT("Replay widget set game time"))
					}else UE_LOG(LogTemp,Error,TEXT("Iterator for game time variable is not valid, size %d"), BallInfo->BallsInfoArray.Num())
				}
			}
		}
	}
}
	// UE_LOG(LogTemp,Error,TEXT("BallInfo Size: %i"),BallInfo->GetBallInfoArray().Num())

void ABallsVisualization::PlayNextStage()
{
	StopVisualize();
	UE_LOG(LogTemp,Error,TEXT("ABallsVisualization::PlayNextStage %i"),CurrentStage)
	// todo заюзатбь для паузы
	if (!CurrentStage || bPreviousViz)
	{
		CurrentStage++;
		bPreviousViz = false;
	}
	ReinitToNextStage();
}

void ABallsVisualization::PlayPreviousStage()
{
	StopVisualize();
	UE_LOG(LogTemp, Error, TEXT("ABallsVisualization::PlayPreviousStage %i"), CurrentStage)
	// todo заюзатбь для паузы
	if (CurrentStage) CurrentStage--;
	TArray<FStageData> FrameBallArray = objects->ReplayManager->GetFrameBallArray();
	BallInfo->BallsInfoArray.Reset();
	shotBeginTime = GetGameTimeSinceCreation();
	
	if (FrameBallArray.IsValidIndex(CurrentStage))
	{
		objects->ballinfo->BallsInfoArray.Append(FrameBallArray[CurrentStage].BallInfoArray);
		// TArray<int32> StagesEnd = objects->ReplayManager->GetStagesEnd();
		// if (StagesEnd.IsValidIndex(CurrentStage)) UE_LOG(LogTemp,Error,TEXT("Reinit stage, Stage end %i"),StagesEnd[CurrentStage])
		
		UE_LOG(LogTemp,Error,TEXT("Added %i"),FrameBallArray[CurrentStage].BallInfoArray.Num())
		if (!bShouldVisualize) StartVisualize();
		// if (ReinitCallCount == 1) return;
		iterator = 0;
		ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
		bPreviousViz = true;
	}
}

void ABallsVisualization::StartVisualize()
{
    if (!bShouldVisualize)
    {
        shotBeginTime = GetGameTimeSinceCreation();
        if (!HasAuthority())
        {
            if (objects && objects->hud && objects->hud->userBarWidget)
            {
                objects->hud->userBarWidget->ReplicationPauseEnd();
            }
        }
    }
    objects->opponent->Disable();
	bShouldVisualize = true;
	UE_LOG(LogTemp,Error,TEXT("Start visualize with stage in array %i"),objects->ReplayManager->GetFrameBallArray().Num())
}

void ABallsVisualization::StopVisualize()
{
	bShouldVisualize = false;
	if (!BallInfo->IsReplayMode())
	{
		BallInfo->BallsInfoArray.Reset();
	}
}

void ABallsVisualization::VisualizeSnapshot(const FBallsInfo& pos)
{
    snapshot = pos;
    GetWorldTimerManager().SetTimer(snapshotDelay,
                                    this,
                                    &ABallsVisualization::TimedVisualizeSnapshot,
                                    0.1f,
                                    true,
                                    0.0f);
    PRINTF((TEXT("viz snapshot"), *(snapshot.BallPos[0].ToString())), Red)
}

void ABallsVisualization::VisualizeSnapshotNow(const FBallsInfo& pos)
{
    for (int i=0; i < BallCountInGame; i++)
    {
        objects->physicsEngine->MoveBallAndStop(i, pos.BallPos[i], pos.BallRotation[i]);
    	// UE_LOG(LogTemp,Error,TEXT("Snapshot visualized"))
    }
}



int32 ABallsVisualization::GetCurrentStage(int32 CurrentFrame)
{
	TArray<int32> StagesEnds = objects->ReplayManager->GetStagesEnd();
	for (int32 i = 0; i < StagesEnds.Num();++i)
	{
		if (i == 0)
		{
			if (CurrentFrame <= StagesEnds[i])
			{
				return i;
			}
		}
		if (StagesEnds.IsValidIndex(i-1))
		{
			if (CurrentFrame > StagesEnds[i-1] && CurrentFrame <= StagesEnds[i])
			{
				return i;
			}
		}
	}
	return INDEX_NONE;
}

void ABallsVisualization::TimedVisualizeSnapshot()
{
    if (bShouldVisualize) return;
    GetWorldTimerManager().ClearTimer(snapshotDelay);
    PRINTF((TEXT("Client side viz cue %s"), *(snapshot.BallPos[0].ToString())), Red)
    VisualizeSnapshotNow(snapshot);
}

void ABallsVisualization::ReinitToNextStage()
{
	TArray<FStageData> FrameBallArray = objects->ReplayManager->GetFrameBallArray();
	TArray<FStageSoundInfo> FrameSoundArray = objects->ReplayManager->GetSoundInfo();
	BallInfo->BallsInfoArray.Reset();
	shotBeginTime = GetGameTimeSinceCreation();
    soundVizHasEnded = false;
	if (bPreviousViz)
	{
		CurrentStage++;
		bPreviousViz = false;
	}
	if (FrameBallArray.IsValidIndex(CurrentStage))
	{
		objects->ballinfo->BallsInfoArray.Append(FrameBallArray[CurrentStage].BallInfoArray);
		BallInfo->SoundsInfoArray.Append(FrameSoundArray[CurrentStage].SoundInfos);
		// TArray<int32> StagesEnd = objects->ReplayManager->GetStagesEnd();
		// if (StagesEnd.IsValidIndex(CurrentStage)) UE_LOG(LogTemp,Error,TEXT("Reinit stage, Stage end %i"),StagesEnd[CurrentStage])
		
		UE_LOG(LogTemp,Error,TEXT("Added %i"),FrameBallArray[CurrentStage].BallInfoArray.Num())
		if (!bShouldVisualize) StartVisualize();
		// if (ReinitCallCount == 1) return;
		iterator = 0;
		ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
		if (BallInfo->IsReplayMode())
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(),0);
			if (PC)
			{
				APoolHUD* HUD = Cast<APoolHUD>(PC->GetHUD());
				if (HUD)
				{
					UReplayWidget* ReplayWidget = Cast<UReplayWidget>(HUD->userBarWidget);
					if (ReplayWidget)
					{
						ReplayWidget->SetActivePlayer(FrameBallArray[CurrentStage].ActivePlayerNum);
					} else UE_LOG(LogTemp,Error,TEXT("Replay widget not valid"))
				}
			}
		}
		CurrentStage++;
	}
	
	
}

int32 ABallsVisualization::GetCurrentVisualizedFrame()
{
	return iterator;
}

void ABallsVisualization::VisualizeFromFrame(int32 Frame)
{
	if (!bShouldVisualize) bShouldVisualize = true;
	if (objects->ballinfo->BallsInfoArray.IsValidIndex(Frame))
	{
		iterator = Frame;
		shotBeginTime = GetGameTimeSinceCreation();
		ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
		
	}
	
}

float ABallsVisualization::GetReplayProgress()
{
	return (float)iterator / (float) (objects->ballinfo->BallsInfoArray.Num() - 1);
}

int32 ABallsVisualization::GetFrameFromProgress(float Progress)
{
	float ClampedProgress = FMath::Clamp(Progress,0.f,0.98f);
	return FMath::RoundToInt((objects->ballinfo->BallsInfoArray.Num() - 1) *ClampedProgress);
}
