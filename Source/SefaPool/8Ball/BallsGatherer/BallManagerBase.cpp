// Fill out your copyright notice in the Description page of Project Settings.


#include "BallManagerBase.h"

#include "SefaPool/Shared/ObjectGatherer.h"


ABallManagerBase::ABallManagerBase()
{
	PrimaryActorTick.bCanEverTick = false;
	FirstBallIndex = 0;
	BallsCountInGame = 16;
	bIsBallsSeted = false;
}

void ABallManagerBase::BeginPlay()
{
	Super::BeginPlay();
	GATHER_OBJECT_GATHERER(ObjectGatherer);
	UE_LOG(LogTemp,Error,TEXT("Ball manager was spawned"))
	
}

void ABallManagerBase::SetBallsForGM()
{
	UE_LOG(LogTemp,Error,TEXT("Set balls"))
	if (bIsBallsSeted) return;
	bIsBallsSeted = true;
	TArray<ABall*> Tmp;
	int BallNumIterator = 0;
	for (int i =0; i<FirstBallIndex;++i)
	{
		ObjectGatherer->balls[i]->Destroy();
	}
	for (int i = FirstBallIndex+BallsCountInGame;i <BallsCountInScene;++i)
	{
		ObjectGatherer->balls[i]->Destroy();
	}
	for (int i = FirstBallIndex;i<FirstBallIndex+BallsCountInGame;++i)
	{
		Tmp.Add(ObjectGatherer->balls[i]);
		Tmp[BallNumIterator]->ballNum = BallNumIterator;
		BallNumIterator++;
	}

	for (int i =0; i<BallsCountInGame;++i)
	{
		ObjectGatherer->balls[i] = Tmp[i];
	}
	ObjectGatherer->physicsEngine->UpdateBallInfo(Tmp);
	ObjectGatherer->ballvisualization->BallCountInGame = BallsCountInGame;
	ObjectGatherer->physicsEngine->BallCountInGame = BallsCountInGame;
	ObjectGatherer->physicsEngine->InitializePhysicEngine();
}

