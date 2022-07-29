// Fill out your copyright notice in the Description page of Project Settings.


#include "ReplayVisualization.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"
#include "SefaPool/Shared/ReplaySystem/ReplaySaveGame.h"

// Sets default values
AReplayVisualization::AReplayVisualization()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bShouldVisualize = false;
	iterator = 0;
	SetReplicates(true);
	bAlwaysRelevant = true;
    shotBeginTime = 0.0f;
	CurrentStage = 0;
}

// Called when the game starts or when spawned
void AReplayVisualization::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(objects)
	BallInfo = objects->ballinfo;
	shotBeginTime = 0.0f;
}

// Called every frame
void AReplayVisualization::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShouldVisualize)
	{
		float currentTime = GetGameTimeSinceCreation() - shotBeginTime;
		while (BallInfo->BallsInfoArray.IsValidIndex(iterator) &&
			BallInfo->BallsInfoArray[iterator].time - ReplayMovableZero < currentTime)
		{
			// UE_LOG(LogTemp,Error,TEXT("Visualize %i frame"),iterator)
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
			UE_LOG(LogTemp,Warning,TEXT("Stop visualize %i"),BallInfo->BallsInfoArray.Num())
			BallInfo->BallsInfoArray.Reset();
			soundIterator = 0;
			iterator = 0;
			BallInfo->SoundsInfoArray.Reset();
			bShouldVisualize = false;
			if (BallInfo->IsReplayMode())
			{
				ReinitToNextStage();
			}
		}
		
	}
}
	// UE_LOG(LogTemp,Error,TEXT("BallInfo Size: %i"),BallInfo->GetBallInfoArray().Num())

void AReplayVisualization::PlayNextStage()
{
	UE_LOG(LogTemp,Error,TEXT("AReplayVisualization::PlayNextStage %i"), iterator)
	TArray<int32> StageEnds = objects->ReplayManager->GetStagesEnd();
	// for (int32 i = 0; i < StageEnds.Num();++i)
	// {
	// 	if (StageEnds.IsValidIndex(i+1))
	// 	{
	// 		if (iterator > StageEnds[i] && iterator < StageEnds[i + 1])
	// 		{
	// 			UE_LOG(LogTemp,Error,TEXT("Iterator %i beetwen %i and %i, set value %i"),iterator,StageEnds[i],StageEnds[i+1],StageEnds[i+1])
	// 			iterator = StageEnds[i + 1] +1;
	// 			ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
	// 			shotBeginTime = GetGameTimeSinceCreation();
	// 			if (!bShouldVisualize) StartVisualize();
	// 		
	// 		}
	// 	}
	// }
	// todo заюзатбь для паузы
	int i = GetCurrentStage(iterator);
	if (StageEnds.IsValidIndex(i+1))
	{
		iterator = StageEnds[i];
		if (BallInfo->BallsInfoArray.IsValidIndex(iterator))
		{
			ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
			shotBeginTime = GetGameTimeSinceCreation();
			if (!bShouldVisualize) StartVisualize();
		}
		
		
	}
	
}

void AReplayVisualization::PlayPreviousStage()
{
	TArray<int32> StageEnds = objects->ReplayManager->GetStagesEnd();
	UE_LOG(LogTemp,Error,TEXT("AReplayVisualization::PlayPreviousStage %i"),iterator)
	// for (int32 i = 0; i < StageEnds.Num();++i)
	// {
	// 	if (StageEnds.IsValidIndex(i+1) )
	// 	{
	// 		if (iterator > StageEnds[i] && iterator < StageEnds[i + 1])
	// 		{
	// 			if (StageEnds.IsValidIndex(i - 1))
	// 			{
	// 				UE_LOG(LogTemp,Error,TEXT("Iterator %i beetwen %i and %i, set value %i"),iterator,StageEnds[i],StageEnds[i+1],StageEnds[i-1])
	// 				iterator = StageEnds[i - 1];
	// 				ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
	// 				shotBeginTime = GetGameTimeSinceCreation();
	// 				if (!bShouldVisualize) StartVisualize();
	// 			}
	// 		}
	// 	} else if (i == StageEnds.Num() - 1 && ! bShouldVisualize)
	// 	{
	// 		iterator = StageEnds[StageEnds.Num() -2 ];
	// 		UE_LOG(LogTemp,Warning,TEXT("Else if called %i Array num %i"),iterator,BallInfo->BallsInfoArray.Num())
	// 		StartVisualize();
	// 		return;
	// 	}
	// }
	int i = GetCurrentStage(iterator);
	if (StageEnds.IsValidIndex(i - 2))
	{
		iterator = StageEnds[i - 2];
		ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
		shotBeginTime = GetGameTimeSinceCreation();
		if (!bShouldVisualize) StartVisualize();
	}
	else
	{
		if (StageEnds.IsValidIndex(i - 1))
		{
			iterator = StageEnds[i - 1];
			ReplayMovableZero = BallInfo->BallsInfoArray[iterator].time;
			shotBeginTime = GetGameTimeSinceCreation();
			if (!bShouldVisualize) StartVisualize();
		}
	}
	
}

void AReplayVisualization::StartVisualize()
{
    if (!bShouldVisualize)
    {
    	UE_LOG(LogTemp,Error,TEXT("Stages end %i"),objects->ReplayManager->GetStagesEnd().Num())
        shotBeginTime = GetGameTimeSinceCreation();
    }
    objects->opponent->Disable();
	bShouldVisualize = true;
}

void AReplayVisualization::StopVisualize()
{
	UE_LOG(LogTemp,Warning,TEXT("Stop visualize called"))
	bShouldVisualize = false;
}

void AReplayVisualization::VisualizeSnapshot(const FBallsInfo& pos)
{
    snapshot = pos;
    GetWorldTimerManager().SetTimer(snapshotDelay,
                                    this,
                                    &AReplayVisualization::TimedVisualizeSnapshot,
                                    0.1f,
                                    true,
                                    0.0f);
    PRINTF((TEXT("viz snapshot"), *(snapshot.BallPos[0].ToString())), Red)
}

void AReplayVisualization::VisualizeSnapshotNow(const FBallsInfo& pos)
{
    for (int i=0; i < BallCountInGame; i++)
    {
        objects->physicsEngine->MoveBallAndStop(i, pos.BallPos[i], pos.BallRotation[i]);
    }
}

int32 AReplayVisualization::GetCurrentStage(int32 CurrentFrame)
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

void AReplayVisualization::TimedVisualizeSnapshot()
{
    if (bShouldVisualize) return;
    GetWorldTimerManager().ClearTimer(snapshotDelay);
    PRINTF((TEXT("Client side viz cue %s"), *(snapshot.BallPos[0].ToString())), Red)
    VisualizeSnapshotNow(snapshot);
}

void AReplayVisualization::ReinitToNextStage()
{
	TArray<FStageData> FrameBallArray = objects->ReplayManager->GetFrameBallArray();
	BallInfo->BallsInfoArray.Reset();

	shotBeginTime = GetGameTimeSinceCreation();
	if (FrameBallArray.IsValidIndex(CurrentStage))
	{
		objects->ballinfo->BallsInfoArray.Append(FrameBallArray[CurrentStage].BallInfoArray);
		UE_LOG(LogTemp,Error,TEXT("Added %i"),FrameBallArray[CurrentStage].BallInfoArray.Num())
		if (!bShouldVisualize) StartVisualize(); UE_LOG(LogTemp,Error,TEXT("Start visuzliae on replay visualizator"))
		CurrentStage++;
	}
	
}
