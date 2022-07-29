// Fill out your copyright notice in the Description page of Project Settings.


#include "EnglishPool.h"

#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"


//class USefaPoolSettings;

AEnglishPool::AEnglishPool()
{
	firstShot = true;
	FirstBallIndex = 16;
	BallCountInGame = 16;
}

bool AEnglishPool::IsSightBallPocketed(int32 BallNum)
{
	UE_LOG(LogTemp,Error,TEXT("Pocketed ball num %i"),BallNum)
	bool Result = false;
	if (BallNum && BallNum <= 7)
	{
		if (PlayersStates[shotInfo.currentPlayerNum].LowPocketed) Result = true;
		if (IsAllYellowBallsPocketed())
		{
			if (PlayersStates[shotInfo.currentPlayerNum].LowPocketed)
			{
				PlayersStates[shotInfo.currentPlayerNum].LowPocketed = false;
				PlayersStates[shotInfo.currentPlayerNum].BlackPocketed = true;
			}else
			{
				PlayersStates[1 - shotInfo.currentPlayerNum].LowPocketed = false;
				PlayersStates[1 - shotInfo.currentPlayerNum].BlackPocketed = true;
			}
		}
		return Result;
	}
	if (BallNum == 8)
	{
		UE_LOG(LogTemp,Error,TEXT("Message pocket black ball"))
		UE_LOG(LogTemp,Error,TEXT("Message opponent pocket black ball"))
		if (PlayersStates[shotInfo.currentPlayerNum].BlackPocketed)
		{
			UE_LOG(LogTemp,Error,TEXT("WIN"))
			shotInfo.shotResult = E_ShotResult::WIN;
		}else
		{
			UE_LOG(LogTemp,Error,TEXT("LOSE"))
			shotInfo.shotResult = E_ShotResult::LOSE;
			return false;
		}
	
		return true;
	}
	if (BallNum >= 9)
	{
		UE_LOG(LogTemp,Error,TEXT("Message pocketed red ball"))
		UE_LOG(LogTemp,Error,TEXT("Opponent pocket red ball"))
		if (PlayersStates[shotInfo.currentPlayerNum].HighPocketed) Result = true;
		if (IsAllRedBallsPocketed())
		{
			if (PlayersStates[shotInfo.currentPlayerNum].HighPocketed)
			{
				PlayersStates[shotInfo.currentPlayerNum].HighPocketed = false;
				PlayersStates[shotInfo.currentPlayerNum].BlackPocketed = true;
			}else
			{
				PlayersStates[1 - shotInfo.currentPlayerNum].HighPocketed = false;
				PlayersStates[1 - shotInfo.currentPlayerNum].BlackPocketed = true;
			}
		}
		return Result;
	}
	if (BallNum == 0)
	{
		UE_LOG(LogTemp,Error,TEXT("Pocketed cue ball"))
		UE_LOG(LogTemp,Error,TEXT("Opponent pocketed cue ball, you get double shot"))
	}
	RegisterMessage(PRIMARY,MESSAGE_POCKETED_OPPONENT_BALL,true,true);
	RegisterMessage(PRIMARY,MESSAGE_OPPONENT_POCKET_YOUR_BALL,false,true);        
	return false;
}

void AEnglishPool::UpdateTargetableBalls(APoolPlayerStateBase* PlayerState)
{
	UE_LOG(LogTemp,Error,TEXT("Update targetable balls on english pool class"))
	TargetableBalls.Reset();
	if (PlayersStates[shotInfo.currentPlayerNum].LowPocketed)
	{
		for (int32 i = 0; i < 8;i++)
		{
			TargetableBalls.Add(i);

		}
		RegisterMessage(PRIMARY,MESSAGE_POCKET_YELLOW_BALL,true,true);
		RegisterMessage(PRIMARY,MESSAGE_OPPONENT_SHOULD_POCKET_YELLOW_BALL,false,true);
		UE_LOG(LogTemp,Error,TEXT("Target balls is low pocketed"))
	}else if (PlayersStates[shotInfo.currentPlayerNum].HighPocketed)
	{
		for (int32 i = 9; i < 16;++i)
		{
			TargetableBalls.Add(i);
		}
		RegisterMessage(PRIMARY,MESSAGE_POCKET_RED_BALL,true,true);
		RegisterMessage(PRIMARY,MESSAGE_OPPONENT_SHOULD_POCKET_RED_BALL,false,true);
		UE_LOG(LogTemp,Error,TEXT("Target balls is hight pocketed"))
	}else if (PlayersStates[shotInfo.currentPlayerNum].BlackPocketed)
	{
		TargetableBalls.Add(8);
		RegisterMessage(PRIMARY,MESSAGE_POCKET_BLACK_BALL,true,true);
		RegisterMessage(PRIMARY,MESSAGE_OPPONENT_SHOULD_POCKET_BLACK_BALL,false,true);
		UE_LOG(LogTemp,Error,TEXT("Target balls is black ball"))
	}
	else if (!PlayersStates[shotInfo.currentPlayerNum].IsSightBallsSet())
	{
		for (int32 i = 1; i < 16; ++i)
		{
			if (i != 8)
			{
				TargetableBalls.Add(i);
			}
		}
	}
	TArray<APoolPlayerControllerBase*> Players;
	if (bIsTeamMode)
	{
		Players = shotInfo.currentPlayerNum == 0 ?  FirstTeam : SecondTeam;
	}
	else Players.Add(playingPcs[shotInfo.currentPlayerNum]);
	for (int32 i = 0; i < Players.Num();i++)
	{
		APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(Players[i]->PlayerState);
		if (PS)
		{
			PS->ClientSetTargetableBalls(TargetableBalls);
		}
	}
	//return TargetableBalls;
}

bool AEnglishPool::IsPlayerSnookered()
{
	UE_LOG(LogTemp,Error,TEXT("EnglishPool::IsPlayerSnookered() %i"),TargetableBalls.Num())
	APoolPlayerControllerBase* PC = Cast<APoolPlayerControllerBase>(playingPcs[shotInfo.currentPlayerNum]);
	float StepDelta = objects->physicsEngine->ballR / 5.f;
	FVector TraceStart = objects->balls[0]->GetActorLocation();
	for (int32 i =0; i < TargetableBalls.Num();++i)
	{
		// UE_LOG(LogTemp,Error,TEXT("Target ball is %i"),TargetableBalls[i])
		FVector StartLocation = objects->balls[TargetableBalls[i]]->GetActorLocation() + objects->balls[TargetableBalls[i]]->GetActorRightVector() * (StepDelta * 5);
		for (int32 j =0; j < 10; ++j)
		{
			if (objects->balls[TargetableBalls[i]]->pocketed) continue;
			StartLocation = StartLocation +  objects->balls[TargetableBalls[i]]->GetActorRightVector() *-StepDelta;
			FVector TraceEnd = TraceStart + UKismetMathLibrary::FindLookAtRotation(TraceStart,StartLocation).Vector() * objects->balls[0]->GetDistanceTo(objects->balls[TargetableBalls[i]]);
			FHitResult HitResult;
			bool TraceResult = GetWorld()->LineTraceSingleByChannel(HitResult,TraceStart,TraceEnd,ECollisionChannel::ECC_Visibility);
			// PC->ClientDrawDebugLine(TraceStart,TraceEnd);
			if (TraceResult)
			{
				if (HitResult.GetActor() && HitResult.GetActor() == objects->balls[TargetableBalls[i]])
				{
					UE_LOG(LogTemp,Error,TEXT("Not snooker position"))
					return true;
				}
			}
			// bool TraceResult = GetWorld()->LineTraceSingleByChannel(GetWorld(),TraceStart,TraceEnd,
			// 	ECollisionChannel::ECC_Visibility,)
		}
	}
	UE_LOG(LogTemp,Error,TEXT("Snooker position"))
	return false;
}


bool AEnglishPool::IsAllRedAdnYellowBallsPocketed()
{
	for (uint8 i = 0; i < BallCountInGame; ++i)
	{
		if (objects->balls[i]->pocketed)
		{
			continue;
		}
		UE_LOG(LogTemp,Error,TEXT("Yellow balls not pocketed"))
		return false;
	}
	UE_LOG(LogTemp,Error,TEXT("Yellow balls pocketed"))
	return true;
}

bool AEnglishPool::IsAllYellowBallsPocketed()
{
	for (int32 i = 1; i <= 7;++i)
	{
		if (objects->balls[i]->pocketed)
		{
			continue;
		}
		UE_LOG(LogTemp,Error,TEXT("Yellow balls not pocketed"))
		return false;
	}
	UE_LOG(LogTemp,Error,TEXT("Yellow balls pocketed"))
	return true;
}

bool AEnglishPool::IsAllRedBallsPocketed()
{
	for (int32 i = 9; i < 16; ++i)
	{
		if (objects->balls[i]->pocketed)
		{
			continue;
		}
		UE_LOG(LogTemp,Error,TEXT("Red balls not pocketed"))
		return false;
	}
	UE_LOG(LogTemp,Error,TEXT("Red balls pocketed"))
	return true;
}

void AEnglishPool::OnPlayTimeEnd()
{
	Super::OnPlayTimeEnd();
}

void AEnglishPool::ProcessBallPocketed(int num, bool first)
{
	// Super::ProcessBallPocketed(num, first);
	StagePocketedBalls.Add(num);
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
	
	if (num == 0)
	{
		PRINTF((TEXT("Returning cue ball")), Blue)
		gs->ReturnCueBall();
	}else
	{
		gs->ballsInGame[num] = false;
		objects->balls[num]->pocketed = true;
	}
}

void AEnglishPool::ProcessBallKnockedOff(int num)
{
	// todo написать 
	Super::ProcessBallKnockedOff(num);
	UE_LOG(LogTemp,Error,TEXT("procell ball knocked off"))
}

void AEnglishPool::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
	shotInfo.readyToFinalize = true;
	if (StagePocketedBalls.Num())
	{
		if (firstShot )
		{

			UE_LOG(LogTemp,Error,TEXT("First shot"))
			for (uint8 i = 0; i < StagePocketedBalls.Num(); ++i)
			{
				if (!PlayersStates[shotInfo.currentPlayerNum].IsSightBallsSet())
				{
					CheckRules_FirstShot_(StagePocketedBalls[i]);
				}
				if (IsSightBallPocketed(StagePocketedBalls[i]))
				{
					UE_LOG(LogTemp,Error,TEXT("Shot retauned"))
					if (shotInfo.shotResult < E_ShotResult::SHOT_RETAINED)
					{
						shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
					}
				}
				else
				{
					UE_LOG(LogTemp,Error,TEXT("Shot FOUL"))
					if (shotInfo.shotResult < E_ShotResult::FOUL)
					{
						shotInfo.shotResult = E_ShotResult::FOUL;
					}
				}
			}
			firstShot = false;
			// return;
		}
		for (int32 i =0; i < StagePocketedBalls.Num();++i)
		{
			UE_LOG(LogTemp,Error,TEXT("Not first shot processing"))
			if (IsSightBallPocketed(StagePocketedBalls[i]))
			{
				if (shotInfo.shotResult < E_ShotResult::SHOT_RETAINED)
				{
					shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
				}
			}
			else
			{
				if (shotInfo.shotResult < E_ShotResult::FOUL)
				{
					UE_LOG(LogTemp,Error,TEXT("Shot FOUL"))
					PlayersStates[1 - shotInfo.currentPlayerNum].bDoubleShot = true;
					RegisterMessage(PRIMARY,MESSAGE_POCKETED_OPPONENT_BALL);
					RegisterMessage(SECONDARY,MESSAGE_OPPONENT_GOT_DOUBLE_SHOT);
					RegisterMessage(PRIMARY,MESSAGE_YOU_GOT_DOUBLE_SHOT,false);
					RegisterMessage(SECONDARY,MESSAGE_OPPONENT_POCKET_YOUR_BALL,false);
					shotInfo.shotResult = E_ShotResult::FOUL;
				}
			}
		}
	}
	else
	{
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
	}
	if (shotInfo.shotResult == E_ShotResult::SHOT_RETAINED && !(PlayersStates[1 - shotInfo.currentPlayerNum].bIsFouled && PlayersStates[shotInfo.currentPlayerNum].bIsSnookered)) //  snooker logic
	{
		// todo уточнить надо ли считать проебом попадание в стол, помимо попадания в левый шар
		UE_LOG(LogTemp,Warning,TEXT("CHECK CUE BALL FIRST HIT"))
		for (int32 i = 0; i < hitLog.Num();++i)
		{
			if (hitLog[i].obj1 == 0)
			{
				if (hitLog[i].obj2 != 117 && !IsSightBallPocketed(hitLog[i].obj2))
				{
					UE_LOG(LogTemp,Warning,TEXT("Obj1 %i Obj2 %i"),hitLog[i].obj1,hitLog[i].obj2)\
					shotInfo.shotResult = E_ShotResult::FOUL;
					return;
				}else if (hitLog[i].obj2 !=117 && IsSightBallPocketed(hitLog[i].obj2))
				{
					return;
				}
			}
		}
	}
}

void AEnglishPool::OnPlayerStageStart()
{
	APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(playingPcs[shotInfo.currentPlayerNum]->PlayerState);
	GetWorld()->GetTimerManager().SetTimer(FirstPlayerPlayDelay, this, &ARefereeBase::OnPlayTimeEnd,PS->cueInfo.GetTime() + 3.0f);
	StagePocketedBalls.Reset();
	UpdateTargetableBalls(PS);
	PlayersStates[shotInfo.currentPlayerNum].bIsSnookered = IsPlayerSnookered();
}

bool AEnglishPool::CanPlayerShoot(APoolPlayerControllerBase* player)
{
	return Super::CanPlayerShoot(player);
}

void AEnglishPool::NextShot(uint8 pc_num)
{
	Super::NextShot(pc_num);
}

void AEnglishPool::OnGameStart()
{
	Super::OnGameStart();
}

void AEnglishPool::CheckRules()
{
	// Super::CheckRules();
}

void AEnglishPool::CheckRules_FirstShot_(uint8 BallNum)
{
	if (BallNum <= 7 && BallNum != 0)
	{
		PlayersStates[shotInfo.currentPlayerNum].LowPocketed = true;
		PlayersStates[1 - shotInfo.currentPlayerNum].HighPocketed = true;
		UE_LOG(LogTemp,Error,TEXT("Sight ball set as LowPocketed"))
	}
	else if (BallNum > 8 && BallNum <= 15)
	{
		PlayersStates[shotInfo.currentPlayerNum].HighPocketed = true;
		PlayersStates[1 - shotInfo.currentPlayerNum].LowPocketed = true;
		UE_LOG(LogTemp,Error,TEXT("Sight ball set as HighPocketed"))
	}
	// todo
	// else if (BallNum == 8)
}


void AEnglishPool::CheckRules_Finalize()
{
	// DeployMessages(true);
	UE_LOG(LogTemp,Error,TEXT("AEnglishPool::CheckRules_Finalize)"))
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*>(pc->PlayerState);
	APool8BallPlayerController* other_pc = Cast<APool8BallPlayerController>(playingPcs[1 - shotInfo.currentPlayerNum]);
	APool8BallPlayerState* other_ps = dynamic_cast<APool8BallPlayerState*>(other_pc->PlayerState);
	PlayersStates[1 - shotInfo.currentPlayerNum].bIsFouled = false;
	uint8 nextPlayer = -1;
	
	if (shotInfo.shotResult == E_ShotResult::SHOT_RETAINED)
	{
		nextPlayer = shotInfo.currentPlayerNum;
		ps->ballInHand = false;
		other_ps->ballInHand = false;
		PlayersStates[shotInfo.currentPlayerNum].bIsFouled = false;
		UE_LOG(LogTemp,Error,TEXT("Shot retained"))
	}
	else if (shotInfo.shotResult == E_ShotResult::FOUL)
	{
		if (PlayersStates[shotInfo.currentPlayerNum].IsPlayerHasDoubleShot())
		{
			nextPlayer = shotInfo.currentPlayerNum;
			PlayersStates[shotInfo.currentPlayerNum].bDoubleShot = false;
			PlayersStates[shotInfo.currentPlayerNum].bIsFouled = true;
			UE_LOG(LogTemp,Error,TEXT("Player has double shot, result:E_ShotResult::FOUL"))
		} else nextPlayer = 1 - shotInfo.currentPlayerNum;
		ps->ballInHand = false;
		other_ps->ballInHand = true;
		UE_LOG(LogTemp,Error,TEXT("Foul"),nextPlayer)
	}
	else if (shotInfo.shotResult == E_ShotResult::SHOT_LOST)
	{
		if (PlayersStates[shotInfo.currentPlayerNum].IsPlayerHasDoubleShot())
		{
			UE_LOG(LogTemp,Error,TEXT("Player has double shot, result:E_ShotResult::SHOT_LOST"))
			nextPlayer = shotInfo.currentPlayerNum;
			PlayersStates[shotInfo.currentPlayerNum].bDoubleShot = false;
		} else nextPlayer = 1 - shotInfo.currentPlayerNum;
		UE_LOG(LogTemp,Error,TEXT("SHOT LOST"))
	}
	else if (shotInfo.shotResult == E_ShotResult::WIN)
	{
		if (!bIsTeamMode)
		{
			pc->ClientTravelToLevel("Authentication", "WinScreen");
			other_pc->ClientTravelToLevel("Authentication", "LoseScreen");
		}else
		{
			TeamTravelToLoseScreen(1 - shotInfo.currentPlayerNum);
			TeamTravelToWinScreen(shotInfo.currentPlayerNum);
		}
		GameResults();
	}
	else if (shotInfo.shotResult == E_ShotResult::LOSE)
	{
		if (!bIsTeamMode)
		{
			pc->ClientTravelToLevel("Authentication", "LoseScreen");
			other_pc->ClientTravelToLevel("Authentication", "WinScreen");
		}else
		{
			TeamTravelToLoseScreen(shotInfo.currentPlayerNum);
			TeamTravelToWinScreen(1 - shotInfo.currentPlayerNum);
		}
		GameResults();
	}
		UE_LOG(LogTemp,Error,TEXT("Check_Rules_Finalize result is %s"),*UEnum::GetValueAsString(shotInfo.shotResult))
	if (nextPlayer != 255)
	{
		APool8BallGameMode* GM = Cast<APool8BallGameMode>(GetWorld()->GetAuthGameMode());
		if (!bIsTeamMode)
		{
			GM->NextShot(nextPlayer);
		}
		else
		{
			NextTeamShot(nextPlayer);
		}
	}	
}

void AEnglishPool::BeginPlay()
{
	Super::BeginPlay();
	objects->ReplayManager->InitializeGameMode(ERecordedGameMode::EnglishPool);
}
