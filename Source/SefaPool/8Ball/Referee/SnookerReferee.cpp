// Fill out your copyright notice in the Description page of Project Settings.


#include "SnookerReferee.h"



#include "SefaPool/Shared/ObjectGatherer.h"
#include "SefaPool/Shared/PoolGameStateBase.h"
#include "SefaPool/Shared/ShotInfo.h"
#include "SefaPool/Shared/ReplaySystem/ReplayManager.h"

// todo Сделать систему штрафов
ASnookerReferee::ASnookerReferee()
{
	FirstBallIndex = 33;
	BallCountInGame = 22;
	ChoosedColoredBall = 0;
	FoulPoint = 0;
	ColoredBallsPos.Add(FVector2D(0.6f,0.5f));
	ColoredBallsPos.Add(FVector2D(0.4f,0.5f));
	ColoredBallsPos.Add(FVector2D(.5f,.5f));
	ColoredBallsPos.Add(FVector2D(0.5f,1.f));
	ColoredBallsPos.Add(FVector2D(0.5f,1.45f));
	ColoredBallsPos.Add( FVector2D(0.5f,1.85f));
}

bool ASnookerReferee::IsPlayerSnookered()
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
    			PC->ClientDrawDebugLine(TraceStart,TraceEnd);
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

bool ASnookerReferee::IsAllRedBallsPocketed()
{
	for (int i =EBallNum::FirstRedBall;i<EBallNum::LastRedBall;++i)
	{
		if (objects->balls[i]->pocketed)
		{
			continue;
		}
		return false; 
	}
	return true;
}

bool ASnookerReferee::IsAllColoredBallsPocketed()
{
	for (int i= EBallNum::Yellow; i <= EBallNum::Black;++i)
	{
		if (objects->balls[i]->pocketed)
		{
			continue;
		}
		return false;
	}
	return true;
}

void ASnookerReferee::ChangeBallPosition(uint8 BallNum, FVector2D Pos)
{
	UE_LOG(LogTemp, Error, TEXT("Colored ball return into game ball id %i"), BallNum)
	FVector BallLocationOnTable;
	APhysicsEngine* PE = objects->physicsEngine;
	BallLocationOnTable = PE->TableCoordsToWorld(Pos);
	objects->balls[BallNum]->mesh->SetCollisionProfileName(TEXT("BallPreset"));
	objects->physicsEngine->MoveBallAndStop(BallNum, BallLocationOnTable, FRotator::ZeroRotator);
	objects->balls[BallNum]->Pocketed() = false;
	APoolGameStateBase* GS = Cast<APoolGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->ballsInGame[BallNum] = true;
	}
}

bool ASnookerReferee::IsPositionFree(uint8 BallNum)
{
	/*APoolPlayerControllerBase* PS = Cast<APoolPlayerControllerBase>(playingPcs[shotInfo.currentPlayerNum]);
	if (PS)
	{
		FVector Pos = objects->physicsEngine->TableCoordsToWorld(BallData[BallNum].BallPos);
		PS->ClientDrawDebug(Pos,Pos);
	}*/
	return objects->physicsEngine->IsPosFree(BallData[BallNum].BallPos);
}

void ASnookerReferee::ReturnColoredBallIntoGame(uint8 BallNum)
{
	if (!IsAllRedBallsPocketed())
	{
		APoolPlayerControllerBase* PC = Cast<APoolPlayerControllerBase>(playingPcs[shotInfo.currentPlayerNum]);
		// Если цветной должен быть выставлен, а его собственная точка занята, то его следует поместить на свободную точку шара наибольшей стоимости.
		int PocketedBallIterator = BallNum;
		for (int i = BallNum; i <= EBallNum::Black; ++i)
		{
			if (IsPositionFree(i))
			{
				ChangeBallPosition(BallNum, BallData[i].BallPos);
				UE_LOG(LogTemp, Warning, TEXT("Spawn position for %i ball is empty"), BallNum)
				return;
			}
		}

		// Если все точки заняты, то цветной должен быть помещен как можно ближе к собственной точке, между этой точкой и самой близкой частью дальнего борта.
		FVector Dir = FindDirectionToNearestTableWalls(
			objects->physicsEngine->TableCoordsToWorld(BallData[BallNum].BallPos));
		FVector BallDefaultPos = objects->physicsEngine->TableCoordsToWorld(BallData[BallNum].BallPos);
		// todo умножить на радиус шара
		FVector NewPos = BallDefaultPos + Dir * 2.f;
		UE_LOG(LogTemp,Error,TEXT("Default pos not free"))
		float MoveDelta = objects->physicsEngine->ballR;
		while(true)
		{
			FVector2D WorldPosToTable = objects->physicsEngine->WorldToTableCoords(NewPos);
			PC->ClientDrawDebug(BallDefaultPos,NewPos);
			if (objects->physicsEngine->IsPosFree(WorldPosToTable))
			{
				UE_LOG(LogTemp,Warning,TEXT("While processing and found result"))
				break;
			}
			NewPos = NewPos + NewPos*MoveDelta;
			UE_LOG(LogTemp,Error,TEXT("While proocessing"))
		}
	
		// todo проверить на валидность точки и сдвигать их на радиус шара
		FVector2D NewPosInTableCoords = objects->physicsEngine->WorldToTableCoords(NewPos);
		
		ChangeBallPosition(BallNum, NewPosInTableCoords);
		return;
	}
	else UE_LOG(LogTemp, Error, TEXT("All red balls was pocketed"))
}

FVector ASnookerReferee::FindDirectionToNearestTableWalls(FVector BallPos)
{
	APoolPlayerControllerBase* PC = Cast<APoolPlayerControllerBase>(playingPcs[shotInfo.currentPlayerNum]);
	// UE_LOG(LogTemp,Warning,TEXT("ASnookerReferee::FindDirectionToNearestTableWalls"))
	// Направление векторов у стола инвертированы
	FVector2D PossInTableCoords = objects->physicsEngine->WorldToTableCoords(BallPos);
	float YDelta;
	float XDelta;
	if (PossInTableCoords.Y > 1.5f)  YDelta = (2.f - PossInTableCoords.Y );
	else if (PossInTableCoords.Y < 0.5) YDelta = PossInTableCoords.Y;
	else YDelta = 1.f;

	if (PossInTableCoords.X > 0.5f) XDelta = (1.f - PossInTableCoords.X);
	else if (PossInTableCoords.X < 0.5f) XDelta = PossInTableCoords.X;
	else XDelta = 0.5f;

	UE_LOG(LogTemp,Warning,TEXT("X: %f, Y: %f"),XDelta,YDelta);
	UE_LOG(LogTemp,Warning,TEXT("X %f"),PossInTableCoords.X)
	UE_LOG(LogTemp,Warning,TEXT("Y %f"),PossInTableCoords.Y)
	if (XDelta < YDelta)
	{
		FVector Dir = PossInTableCoords.X > 0.5 ? objects->table->GetActorRightVector()*100.f : -objects->table->GetActorRightVector()*100.f;
		PC->ClientDrawDebug(BallPos, BallPos+ Dir);
		return PossInTableCoords.X > 0.5 ? -objects->table->GetActorRightVector() : objects->table->GetActorRightVector();
	}
	else 
	{
		FVector Dir = PossInTableCoords.Y > 1.5 ? -objects->table->GetActorForwardVector()*100.f : objects->table->GetActorForwardVector()*100.f;
		PC->ClientDrawDebug(BallPos,BallPos +Dir);
		return PossInTableCoords.Y > 1.5 ? -objects->table->GetActorForwardVector() : objects->table->GetActorForwardVector();
	}
}

uint8 ASnookerReferee::GetSightingColoredBallIndex()
{
	for (uint8 i = EBallNum::Yellow;i <= EBallNum::Black;++i)
	{
		if (objects->balls[i]->pocketed)
		{
			continue;
		}
		return i;
	}
	return 0;
}

int32 ASnookerReferee::GetBallCost(uint8 BallNum)
{
	if (BallNum>=EBallNum::FirstRedBall && BallNum <=EBallNum::LastRedBall) return 1;
	int Point[6] = {2,3,4,5,6,7};
	UE_LOG(LogTemp,Warning,TEXT("Ball num is %i"),BallNum)
	UE_LOG(LogTemp,Warning,TEXT("Ball cost %i"),Point[FMath::Abs(EBallNum::Yellow-BallNum)])
	return Point[FMath::Abs(EBallNum::Yellow-BallNum)];
}

void ASnookerReferee::BeginPlay()
{
	Super::BeginPlay();
	shotInfo.Clear();
	objects->ReplayManager->InitializeGameMode(ERecordedGameMode::Snooker);
}

void ASnookerReferee::OnGameStart()
{
	Super::OnGameStart();
	shotInfo.openTable = true;
}


void ASnookerReferee::OnPlayTimeEnd()
{
    playingPcs[shotInfo.currentPlayerNum]->ShotTimeHasEnded();
    shotInfo.shotResult = E_ShotResult::FOUL;
	FoulPoint = 4;
    CheckRules_Finalize();
}

void ASnookerReferee::ProcessBallPocketed(int num, bool first)
{
	// if (IsReadyToFinalize()) { return;}
	PocketedBallInFrame.Add(num);
	if ( num > EBallNum::LastRedBall && num <= EBallNum::Black)
	{
		PocketedColoredBallInFrame.Add(num);
	}	
	UE_LOG(LogTemp, Warning, TEXT("BALL POCKETED %i"), num)
}

void ASnookerReferee::ProcessBallKnockedOff(int num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
	
	if (num == EBallNum::CueBall)
	{
		gs->ReturnCueBall();
		shotInfo.cueKnocked = true;
	}
	else
	{
		gs->ReturnFoulBall(num);
	}
	UE_LOG(LogTemp,Warning,TEXT("BALL KNOCKET OFF %i"),num)
}

void ASnookerReferee::ProcessHitLogAndFinalize(const TArray<FHitInfo>& hitLog)
{
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = Cast<APool8BallPlayerState>(pc->PlayerState);
	APool8BallPlayerController* other_pc = Cast<APool8BallPlayerController>(playingPcs[1 - shotInfo.currentPlayerNum]);
	APool8BallPlayerState* other_ps = Cast<APool8BallPlayerState>(other_pc->PlayerState);
    
	CheckSnookerRules();
	if (shotInfo.shotResult != E_ShotResult::FOUL && shotInfo.shotResult != E_ShotResult::SHOT_RETAINED)
	{
		shotInfo.shotResult = E_ShotResult::SHOT_LOST;
		// UE_LOG(LogTemp,Warning,TEXT("Result set to lost"))
	}
	if (PlayerStats->PocketedBallsInFrame >= 2)
	{
		shotInfo.shotResult = E_ShotResult::FOUL;
	}
	if (IsAllRedBallsPocketed())
	{
		ChoosedColoredBall = GetSightingColoredBallIndex();
		UE_LOG(LogTemp,Warning,TEXT("All red balls pocketed, Sight colored ball index %i"),ChoosedColoredBall)
	}
	shotInfo.readyToFinalize = true;
	PlayerStats->PocketedBallsInFrame = 0;
	PocketedBallInFrame.Reset();
	// Если есть больше одного цветного, которые надо выставить, а их собственные точки заняты, тоhitLog шар большей стоимости должен иметь приоритет в порядке выставления.
	if (PocketedColoredBallInFrame.Num())
	{
		if (PocketedBallInFrame.Num() == 1)
		{
			ReturnColoredBallIntoGame(PocketedColoredBallInFrame[0]);
			return;
		}
		PocketedColoredBallInFrame.Sort([] (const uint8 & Lhs,const uint8& Rhs) { return Lhs > Rhs; });
		for (int i =0;i< PocketedColoredBallInFrame.Num();++i)
		{
			ReturnColoredBallIntoGame(PocketedColoredBallInFrame[i]);
		}
	}
	PocketedColoredBallInFrame.Reset();
	UE_LOG(LogTemp,Error,TEXT("Process hit log finalize"))
	
	
}

void ASnookerReferee::ShuffleAndArrangeBalls()
{
	Super::ShuffleAndArrangeBalls();
	int PosIterator = 0;
	for	(uint8 i = EBallNum::Yellow;i <= EBallNum::Black;i++)
	{
		BallData.Add(i,FColoredBallData(ColoredBallsPos[PosIterator],PosIterator+2));
		PosIterator++;
		FVector BallPos = objects->physicsEngine->TableCoordsToWorld(BallData[i].BallPos);
		objects->physicsEngine->MoveBallAndStop(i,BallPos,FRotator::ZeroRotator);
	}
	FVector BallPos = objects->physicsEngine->TableCoordsToWorld(FVector2D(0.5f,0.4f));
	objects->physicsEngine->MoveBallAndStop(0,BallPos,FRotator::ZeroRotator);
}

bool ASnookerReferee::CanPlayerShoot(APoolPlayerControllerBase* player)
{
	if (shotInfo.currentPlayerNum >= 0 &&
        shotInfo.currentPlayerNum <= 1 &&
        playingPcs[shotInfo.currentPlayerNum] == player)
        	return true;
	else
		return false;
}

void ASnookerReferee::NextShot(uint8 pc_num)
{
	APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*> (GetWorld()->GetGameState());
	if (gs)
	{
		if (firstShot)
			gs->SetRuleset(RULES_BEHIND_HEADSTRING);
		else
			gs->SetRuleset(RULES_ANYWHERE);
	}
}

bool ASnookerReferee::IsReadyToFinalize()
{
	return shotInfo.readyToFinalize;
}

void ASnookerReferee::CheckSnookerRules()
{
	// UE_LOG(LogTemp,Warning,TEXT("ASnookerReferee::CheckSnookerRules"))
	APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(playingPcs[shotInfo.currentPlayerNum]->PlayerState);
	APoolPlayerStateBase* OtherPlayerPS = Cast<APoolPlayerStateBase>(playingPcs[GetOtherPlayerID()]->PlayerState);
	bool bPocketedMoreThanOneBall = PocketedBallInFrame.Num() > 1;
	if (bPocketedMoreThanOneBall)
	{

		RegisterMessage(PRIMARY,MESSAGE_POCKETED_MORE_THAN_ONE_BALL);
		RegisterMessage(E_MessageSeverity::SECONDARY,MESSAGE_OPPONENT_POCKET_MORE_THAN_ONE_BALL,false);
		// PS->RegisterMessage(E_MessageSeverity::PRIMARY,MESSAGE_POCKETED_MORE_THAN_ONE_BALL);
		// OtherPlayerPS->RegisterMessage(E_MessageSeverity::SECONDARY,MESSAGE_OPPONENT_POCKET_MORE_THAN_ONE_BALL);
		// хз вроде по правилам
		shotInfo.shotResult = E_ShotResult::FOUL;
		FoulPoint = 4;
		int32 CueBallIndex = 0;
		PocketedBallInFrame.Find(0,CueBallIndex);
		if (CueBallIndex !=INDEX_NONE)
		{
			APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
			gs->ReturnCueBall();
		}
		return;
	}
	for (int i = 0; i < PocketedBallInFrame.Num(); ++i)
	{
		UE_LOG(LogTemp,Error,TEXT("Proccesing ball %i"),PocketedBallInFrame[i])
		int num = PocketedBallInFrame[i];
		PlayerStats[shotInfo.currentPlayerNum].PocketedBallsInFrame++;
		if (!num)
		{
			APoolGameStateBase* gs = dynamic_cast<APoolGameStateBase*>(GetWorld()->GetGameState());
			shotInfo.cuePocketed = true;
			gs->ReturnCueBall();
			if (4 > FoulPoint)
			{
				FoulPoint = 4;
			}
			shotInfo.shotResult = E_ShotResult::FOUL;
			UE_LOG(LogTemp, Warning, TEXT("Pocketed cue ball"))
			if (PS)
			{
				RegisterMessage(PRIMARY,MESSAGE_POCKETED_CUE_BALL);
				RegisterMessage(SECONDARY,MESSAGE_OPPONENT_POCKETED_CUE_BALL,false);
				// PS->RegisterMessage(E_MessageSeverity::PRIMARY,MESSAGE_POCKETED_CUE_BALL);
				// OtherPlayerPS->RegisterMessage(E_MessageSeverity::SECONDARY,MESSAGE_OPPONENT_POCKETED_CUE_BALL);
			}
			
			shotInfo.readyToFinalize = true;
			continue;
		}
		if (IsAllRedBallsPocketed())
		{
			UE_LOG(LogTemp, Warning, TEXT("All Red ball pocketed "))
			if (ChoosedColoredBall)
			{
				if (num == ChoosedColoredBall)
				{
					if (shotInfo.shotResult != E_ShotResult::FOUL) shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
					if (PS && OtherPlayerPS)
					{
						uint16 MessageID = MESSAGE_POCKETED_YELLOW_BALL + (ChoosedColoredBall - 16);
						RegisterMessage(PRIMARY,MessageID);
						// PS->RegisterMessage(E_MessageSeverity::PRIMARY, MessageID);
						MessageID = MESSAGE_OPPONENT_POCKET_YELLOW_BALL + (ChoosedColoredBall - 16);
						RegisterMessage(SECONDARY,MessageID,false);
						// OtherPlayerPS->RegisterMessage(E_MessageSeverity::SECONDARY, MessageID);
						FramePoints = GetBallCost(num);
						continue;
					}
				}
				else
				{
					shotInfo.shotResult = E_ShotResult::FOUL;
					if (FoulPoint < GetBallCost(num) && GetBallCost(num) > 4)
					{
						FoulPoint = GetBallCost(num);
					}
					else FoulPoint = 4;
					continue;
				}
			}
		}
		if (num < 16 && num)
		{
			if (PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall)
			{
				shotInfo.shotResult = E_ShotResult::FOUL;
				UE_LOG(LogTemp, Warning, TEXT("Pocket red ball again"))
				FoulPoint = 7;
				if (!bPocketedMoreThanOneBall)
				{
					RegisterMessage(PRIMARY,MESSAGE_YOU_POCKET_RED_BALL_INSTEAD_COLORED_BALL);
					// PS->RegisterMessage(E_MessageSeverity::PRIMARY,MESSAGE_YOU_POCKET_RED_BALL_INSTEAD_COLORED_BALL);
				}
				continue;
			}
			PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall = true;
			FramePoints = GetBallCost(num);
			if (shotInfo.shotResult != E_ShotResult::FOUL) shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
			UE_LOG(LogTemp, Warning, TEXT("pocketed RedBall"))
			continue;
			// PlayerStats[shotInfo.currentPlayerNum].Points+= FramePoints;
		}
		else
		{
			bool bIsPlayerPocketRedBall = true;
			if (!PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall)
			{
				bIsPlayerPocketRedBall = false;
				shotInfo.shotResult = E_ShotResult::FOUL;
			}
			if (num >= EBallNum::Yellow && num <= EBallNum::Black)
			{
				if (!bIsPlayerPocketRedBall)
				{
					// ReturnColoredBallIntoGame(num);
					UE_LOG(LogTemp, Warning, TEXT("Pocket colored ball, but not pocket red ball before"))
					if (!bPocketedMoreThanOneBall)
					{
						RegisterMessage(PRIMARY,MESSAGE_YOU_POCKET_COLORED_BALL_INSTEAD_RED_BALL);	
						// PS->RegisterMessage(E_MessageSeverity::PRIMARY,MESSAGE_YOU_POCKET_COLORED_BALL_INSTEAD_RED_BALL);
					}
					FoulPoint = 7;
					continue;
				}
				// ReturnColoredBallIntoGame(num);
				FramePoints = GetBallCost(num);
				if (shotInfo.shotResult != E_ShotResult::FOUL) shotInfo.shotResult = E_ShotResult::SHOT_RETAINED;
				PlayerStats[shotInfo.currentPlayerNum].Clear();
				UE_LOG(LogTemp, Warning, TEXT("Pocket colored ball"))
			}

			// PlayerStats[shotInfo.currentPlayerNum].Points+= FramePoints;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Player num %i got %i points, total points %i"), shotInfo.currentPlayerNum+1,
			FramePoints,
			PlayerStats[shotInfo.currentPlayerNum].Points)
}

void ASnookerReferee::CheckRules()
{
	
}

void ASnookerReferee::OnPlayerStageStart()
{
	Super::OnPlayerStageStart();
	APoolPlayerStateBase* PS = Cast<APoolPlayerStateBase>(playingPcs[shotInfo.currentPlayerNum]->PlayerState);
	APoolPlayerStateBase* OtherPlayerPS = Cast<APoolPlayerStateBase>(playingPcs[GetOtherPlayerID()]->PlayerState);
	if (PS && OtherPlayerPS)
	{
		UE_LOG(LogTemp,Error,TEXT("ASnookerReferee::OnPlayerStageStart"))
		UpdateTargetableBalls(PS);
		if (IsAllRedBallsPocketed() && ChoosedColoredBall)
		{
			uint16 MessageID = MESSAGE_POCKET_YELLOW_BALL + (ChoosedColoredBall-16);
			PS->RegisterMessage(E_MessageSeverity::PRIMARY,MessageID);
			// UE_LOG(LogTemp,Warning,TEXT("Message id %i"),MessageID)
			MessageID = MESSAGE_OPPONENT_SHOULD_POCKET_YELLOW_BALL + (ChoosedColoredBall -16);
			OtherPlayerPS->RegisterMessage(E_MessageSeverity::SECONDARY,MessageID);
		}
		else if (!IsAllRedBallsPocketed())
		{
			uint16 MessageID = PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall ?  MESSAGE_POCKET_COLORED_BALL: MESSAGE_POCKET_RED_BALL;
			// PS->RegisterMessage(E_MessageSeverity::PRIMARY,MessageID);
			RegisterMessage(PRIMARY,MessageID);
			MessageID = PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall ? MESSAGE_OPPONENT_SHOULD_POCKET_COLORED_BALL : MESSAGE_OPPONENT_SHOULD_POCKET_RED_BALL;
			// OtherPlayerPS->RegisterMessage(E_MessageSeverity::SECONDARY,MessageID);
			RegisterMessage(SECONDARY,MessageID,false);
		}
		
		DeployMessages(true);
	}
}

void ASnookerReferee::UpdateTargetableBalls(APoolPlayerStateBase* PlayerState)
{
	TargetableBalls.Reset();
	if (IsAllRedBallsPocketed() && ChoosedColoredBall)
	{
		TargetableBalls.Add(ChoosedColoredBall);
		UE_LOG(LogTemp,Error,TEXT("All red balls was pocketed, %i is targetable ball"),ChoosedColoredBall)
	}
	else if (PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall)
	{
		UE_LOG(LogTemp,Error,TEXT("Targetable balls is colored balls"))
		for (int32 i = EBallNum::Yellow; i <= EBallNum::Black;++i)
		{
			TargetableBalls.Add(i);
		}
	}
	else if (!PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall)
	{
		UE_LOG(LogTemp,Error,TEXT("Targetable balls is all red balls"))
		for (int32 i =0;i <= EBallNum::LastRedBall;++i)
		{
			TargetableBalls.Add(i);
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

void ASnookerReferee::CheckRules_Finalize()
{
	APool8BallPlayerController* pc = Cast<APool8BallPlayerController>(playingPcs[shotInfo.currentPlayerNum]);
	APool8BallPlayerState* ps = dynamic_cast<APool8BallPlayerState*>(pc->PlayerState);
	APoolPlayerStateBase* OtherPlayerPS = Cast<APoolPlayerStateBase>(playingPcs[GetOtherPlayerID()]->PlayerState);
	APool8BallPlayerController* other_pc = Cast<APool8BallPlayerController>(playingPcs[1 - shotInfo.currentPlayerNum]);
	APool8BallPlayerState* other_ps = dynamic_cast<APool8BallPlayerState*>(other_pc->PlayerState);
	
	uint8 nextPlayer = -1;

	if (firstShot) firstShot = false;
	if (IsAllRedBallsPocketed() && IsAllColoredBallsPocketed())
	{
		if (PlayerStats[shotInfo.currentPlayerNum].Points > PlayerStats[1 - shotInfo.currentPlayerNum].Points)
		{
			if (!bIsTeamMode)
			{
				if (pc) pc->ClientTravelToLevel("Authentication", "WinScreen");;
				if (other_pc) other_pc->ClientTravelToLevel("Authentication", "LoseScreen");
			}else
			{
				TeamTravelToLoseScreen(1 - shotInfo.currentPlayerNum);
				TeamTravelToWinScreen(shotInfo.currentPlayerNum);
			}
			GameResults();
		}else
		{
			if (!bIsTeamMode)
			{
				if (pc) pc->ClientTravelToLevel("Authentication", "LoseScreen");
				if(other_pc) other_pc->ClientTravelToLevel("Authentication", "WinScreen");
			}else
			{
				TeamTravelToLoseScreen(1 - shotInfo.currentPlayerNum);
				TeamTravelToWinScreen(shotInfo.currentPlayerNum);
			}
			GameResults();
		}
	}
	if (shotInfo.shotResult == E_ShotResult::WIN)
	{
		if (!bIsTeamMode)
		{
			pc->ClientTravelToLevel("Authentication", "WinScreen");;
			other_pc->ClientTravelToLevel("Authentication", "LoseScreen");
		}else
		{
			TeamTravelToLoseScreen(1 - shotInfo.currentPlayerNum);
			TeamTravelToWinScreen(shotInfo.currentPlayerNum);
		}
		GameResults();
		// UE_LOG(LogTemp,Error,TEXT("Win"),nextPlayer)
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
		// UE_LOG(LogTemp,Error,TEXT("Lose"),nextPlayer)
	}
	else if (shotInfo.shotResult == E_ShotResult::SHOT_RETAINED)
	{
		nextPlayer = shotInfo.currentPlayerNum;
		UE_LOG(LogTemp,Error,TEXT("shot info shot result, shot result shot retained"))
		ps->ballInHand = false;
		other_ps->ballInHand = false;
		if (FramePoints)
		{
			PlayerStats[shotInfo.currentPlayerNum].Points += FramePoints;
			UE_LOG(LogTemp,Error,TEXT("points to add %i"),FramePoints)
		}
		FramePoints = 0;
		if (!IsAllRedBallsPocketed())
		{
			if (ps && PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall)
			{
				// ps->RegisterMessage(E_MessageSeverity::PRIMARY, MESSAGE_POCKETED_RED_BALL);
				RegisterMessage(PRIMARY,MESSAGE_POCKETED_RED_BALL);
				// UE_LOG(LogTemp,Error,TEXT("Message sended"))
			}
			else if (pc && !PlayerStats[shotInfo.currentPlayerNum].bPocketedRedBall)
			{
				// ps->RegisterMessage(E_MessageSeverity::PRIMARY,MESSAGE_POCKETED_COLORED_BALL);
				RegisterMessage(PRIMARY,MESSAGE_POCKETED_COLORED_BALL);
				// UE_LOG(LogTemp,Error,TEXT("Message sended"))
			}
		}
	}
	else if (shotInfo.shotResult == E_ShotResult::SHOT_LOST)
	{
		nextPlayer = 1 - shotInfo.currentPlayerNum;
		ps->ballInHand = false;
		other_ps->ballInHand = false;
		UE_LOG(LogTemp,Error,TEXT("Shot Lost"),nextPlayer)
		PlayerStats[shotInfo.currentPlayerNum].Clear();
	}
	else if (shotInfo.shotResult == E_ShotResult::FOUL)
	{
		PlayerStats[shotInfo.currentPlayerNum].Clear();
		FramePoints = 0;
		PlayerStats[1 - shotInfo.currentPlayerNum].Points += FoulPoint;
		nextPlayer = 1 - shotInfo.currentPlayerNum;
		ps->ballInHand = false;
		other_ps->ballInHand = true;
		UE_LOG(LogTemp,Error,TEXT("Foul, player lost in this frame %i poinst"),FoulPoint)
	}
	UE_LOG(LogTemp,Error,TEXT("NextPlayer Value is : %i"),shotInfo.currentPlayerNum)
	ps->UpdatePocketed();
	other_ps->UpdatePocketed();
	ps->UpdateClient();
	other_ps->UpdateClient();
	FoulPoint = 0;
	UE_LOG(LogTemp,Warning,TEXT("Player %i, has %i points"),shotInfo.currentPlayerNum+1,PlayerStats[shotInfo.currentPlayerNum].Points)
	if (nextPlayer != -1)
	{
		APool8BallGameMode* GM = Cast<APool8BallGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			shotInfo.shotResult = E_ShotResult::UNDETERMINED;
			DeployMessages(true);
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
}

