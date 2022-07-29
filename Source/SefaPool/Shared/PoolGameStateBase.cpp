
#include "PoolGameStateBase.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Kismet/GameplayStatics.h"
#include "ObjectGatherer.h"
#include "PoolGameModeBase.h"
#include "Components/TextBlock.h"
#include "SefaPool/8Ball/Referee/RefereeBase.h"
#include "SefaPool/Widgets/PoolMPUserBarWidget.h"
#include "SefaPool/Widgets/PoolSPUserBarWidget.h"

#include "GameFramework/WorldSettings.h"

// void APoolGameStateBase::OnRep_OnFirstPSInit()
// {
// 	UE_LOG(LogTemp,Error,TEXT("OnRep_OnFirstPSInit"))
// 	APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(
// 		UGameplayStatics::GetPlayerController(GetWorld(), 0));
// 	if (pc)
// 	{
// 		APoolHUD* hud = dynamic_cast<APoolHUD*>(pc->GetHUD());
// 		if (hud && hud->userBarWidget)
// 		{
// 			bool bIsOpponent = pc->IsSpectator() ? false : pc->PlayerState == FirstPlayerPS ? false : true;
// 			hud->userBarWidget->UpdateUsername(FirstPlayerPS->username,
// 											bIsOpponent);
// 			UE_LOG(LogTemp,Error,TEXT("OnRep_OnFirstPSInit username: %s"),*FirstPlayerPS->username)
// 		}
// 	}
// }
//
// void APoolGameStateBase::OnRep_OnSecondPSInit()
// {
// 	UE_LOG(LogTemp,Error,TEXT("OnRep_OnSecondPSInit"))
// 	APoolPlayerControllerBase* pc = dynamic_cast<APoolPlayerControllerBase*>(
// 		UGameplayStatics::GetPlayerController(GetWorld(), 0));
// 	if (pc)
// 	{
// 		APoolHUD* hud = dynamic_cast<APoolHUD*>(pc->GetHUD());
// 		if (hud && hud->userBarWidget)
// 		{
// 			bool bIsOpponent = pc->IsSpectator() ? true : pc->PlayerState == SecondPlayerPS ? false : true;
// 			hud->userBarWidget->UpdateUsername(SecondPlayerPS->username,
// 											bIsOpponent);
// 			UE_LOG(LogTemp,Error,TEXT("OnRep_OnSecondPSInit username: %s"),*SecondPlayerPS->username)
// 		}
// 	}
// }



APoolGameStateBase::APoolGameStateBase() :
    rules (ShotRuleset::RULES_BEHIND_HEADSTRING),
	physicsSetup(nullptr),
    objects (nullptr),
	bReadyBallsAndTable(false),
	pocketedStorage(FVector(1802.951172, 1489.594238, 926.984375)),
	ballsInGame()
{
	for (int i = 0; i < 22; i++)
		ballsInGame[i] = true;
    alreadyBegun = false;
	SpectatorCount = 0;
	GameTime = 0;
}

void APoolGameStateBase::Multicast_UpdateUserBarWidget_Implementation()
{
	if (HasAuthority()) return;
	bool ok = true;
	APool8BallPlayerController* PC = Cast<APool8BallPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(),0));
	
	if (ok && !objects) ok = false;
	if (ok && !objects->hud) ok = false;
	if (ok && !objects->hud->userBarWidget) ok = false;
	UPoolMPUserBarWidget* wid = dynamic_cast<UPoolMPUserBarWidget*> (objects->hud->userBarWidget);
	if (ok && !wid) ok = false;
	if (!ok) return;
    
	if (ok && !PC->PlayerState) ok = false;
	APool8BallPlayerState* this_ps = dynamic_cast<APool8BallPlayerState*> (PC->PlayerState);
	if (ok && !this_ps) ok = false;
    
	if (!ok) return;
	
	auto players = PlayerArray;
	int i = 0;
	APool8BallPlayerState* that_ps = nullptr;
	if (PC->IsSpectator())
	{
		this_ps = FirstPlayerPS;
		that_ps = SecondPlayerPS;
	}
	else
	{
		if (PC->PlayerState == FirstPlayerPS)
		{
			this_ps = FirstPlayerPS;
			that_ps = SecondPlayerPS;
		}else 
		{
			this_ps = SecondPlayerPS;
			that_ps = FirstPlayerPS;
		}
	}
	
	if (!ok && !that_ps) ok = false;
    
	if (!ok) return;
    
	TArray<int32> thisNextBalls;
	this_ps->CurrentFrameNextBallsToHit(thisNextBalls);
    
	TArray<int32> thatNextBalls;
	that_ps->CurrentFrameNextBallsToHit(thatNextBalls);
    
	E_Pool8BallNextTarget thisNextTarget = this_ps->nextTarget;
	E_Pool8BallNextTarget thatNextTarget = that_ps->nextTarget;
    
	wid->UpdateUserTargetMode(thisNextTarget);
	wid->UpdateOpponentTargetMode(thatNextTarget);
    
	wid->UpdateUserTargetBalls(thisNextBalls);
	wid->UpdateOpponentTargetBalls(thatNextBalls);
}


void APoolGameStateBase::ExecuteGameTime()
{
	GetWorld()->GetTimerManager().SetTimer(GameTime_Timer,this,&APoolGameStateBase::UpdateGameTime,1.f,true);
	UE_LOG(LogTemp,Error,TEXT("EXECUTE GAME TIME"))
}

void APoolGameStateBase::UpdateGameTime()
{
	GameTime++;
}

void APoolGameStateBase::StopGameTime()
{
	GetWorld()->GetTimerManager().ClearTimer(GameTime_Timer);
}

void APoolGameStateBase::OnRep_OnGameTimeUpdate()
{
	UPoolMPUserBarWidget* Widget = Cast<UPoolMPUserBarWidget>(objects->hud->userBarWidget);
	if (Widget)
	{
		Widget->UpdateGameTime(GameTime);
	}
	else
	{
		UE_LOG(LogTemp,Error,TEXT("Can't update game time"))
	}
}


void APoolGameStateBase::OnRep_OnSpecCountChanged()
{
	UE_LOG(LogTemp,Error,TEXT("Spectator count %i"),SpectatorCount)
	UPoolMPUserBarWidget* Widget = Cast<UPoolMPUserBarWidget>(objects->hud->userBarWidget);
	if (Widget)
	{
		Widget->SpecCountText->SetText(FText::FromString(FString::FromInt(SpectatorCount)));
	}
}

void APoolGameStateBase::BeginPlay()
{
    if (!this) return;
    Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects)
    
    if (objects->ballRestLocation)
        pocketedStorage = objects->ballRestLocation->GetLocation() + FVector(0.0f, 0.0f, objects->balls[0]->scale*50.0f);
    bReadyBallsAndTable = true;
    if (objects->shotinfo)
        objects->shotinfo->ballsInGame = ballsInGame;
	
}

FVector APoolGameStateBase::GetCueBallDefaultLocationOnTable()
{
	return objects->table->GetCueBallDefaultPos();
}

FVector APoolGameStateBase::GetFoulBallDefaultReturnLocationOnTable()
{
	return objects->table->GetFoulDefaultPos();
}

FVector APoolGameStateBase::GetPocketedBallStorageLocation(int num)
{
	return pocketedStorage - FVector(1.0f, 0.0f, 0.0f) * objects->balls[0]->scale * 100.0f * num;
}

void APoolGameStateBase::ReturnCueBall()
{
	if (!this)
		ERROR_DEBUG_PRINT_FUNC
	FVector loc = GetCueBallDefaultLocationOnTable() + FVector(0.0f, 0.0f, 50.0f * objects->balls[0]->scale);
	FVector moveVec = GetCueBallDefaultLocationOnTable() - GetFoulBallDefaultReturnLocationOnTable();
	moveVec.Normalize();
	/*TArray < TEnumAsByte < EObjectTypeQuery > > objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> overlappedActors;
	TArray<AActor*> actorsToIgnore;*/
	while (true)
	{
		/*overlappedActors.Empty();
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(),
			loc,
			100.0f * objects->balls[0]->scale,
			objectTypes,
			ABall::StaticClass(),
			actorsToIgnore,
			overlappedActors);
		if (overlappedActors.Num() == 0) break;*/
        if (objects->physicsEngine->IsPosFree(objects->physicsEngine->WorldToTableCoords(loc)))
            break;
		loc += moveVec * 0.001f;
	}
	//balls[0]->mesh->SetSimulatePhysics(false);
    PRINTF((TEXT("Server side return cue %s"), *(loc.ToString())), Red)
    objects->physicsEngine->MoveBallAndStop(0, loc, FRotator(0.0f));
    objects->balls[0]->mesh->SetCollisionProfileName(TEXT("BallPreset"));
    objects->balls[0]->Pocketed() = false;
	//InfoUpload_LoadBallTransform(0, loc, FRotator(0.0f), true, true, false);

}
void APoolGameStateBase::ReturnFoulBall(int num)
{
	if (!this || !objects || !(objects->balls + num))
		ERROR_DEBUG_PRINT_FUNC
		FVector loc = GetFoulBallDefaultReturnLocationOnTable() + FVector(0.0f, 0.0f, 50.0f * objects->balls[0]->scale);
	FVector moveVec = GetFoulBallDefaultReturnLocationOnTable() - GetCueBallDefaultLocationOnTable();
	moveVec.Normalize();
	/*TArray < TEnumAsByte < EObjectTypeQuery > > objectTypes;
	objectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*> overlappedActors;
	TArray<AActor*> actorsToIgnore;*/
	while (true)
	{
		/*overlappedActors.Empty();
		UKismetSystemLibrary::SphereOverlapActors(GetWorld(),
			loc,
			50.0f * objects->balls[num]->scale,
			objectTypes,
			ABall::StaticClass(),
			actorsToIgnore,
			overlappedActors);
		if (overlappedActors.Num() == 0) break;*/
        if (objects->physicsEngine->IsPosFree(objects->physicsEngine->WorldToTableCoords(loc)))
            break;
		loc += moveVec * 0.001f;
	}
    objects->physicsEngine->MoveBallAndStop(num, loc, FRotator(0.0f));
    objects->balls[num]->mesh->SetCollisionProfileName(TEXT("BallPreset"));
    objects->balls[num]->Pocketed() = false;
	//InfoUpload_LoadBallTransform(num, loc, FRotator(0.0f), false);
}

void APoolGameStateBase::StorePocketedBall(int num)
{
    PRINTF((TEXT("Store pocketed")), Red)
	FVector loc = GetPocketedBallStorageLocation(num);
    objects->balls[num]->Pocketed() = true;
    objects->physicsEngine->MoveBallAndStop(num, loc, FRotator(0.0f, 90.0f, 0.0f));
    //objects->balls[num]->mesh->SetCollisionProfileName(TEXT("NoCollision"));
}

void APoolGameStateBase::ShuffleAndArrangeBalls()
{
    if (!HasAuthority()) return;
	int leftCornerBallNum = FMath::RandRange(1, 7);
	int rightCornerBallNum = FMath::RandRange(9, 15);
	if (FMath::RandRange(0, 1))
	{
		int tmp = leftCornerBallNum;
		leftCornerBallNum = rightCornerBallNum;
		rightCornerBallNum = tmp;
	}
	TArray<int> arrangeableBalls;
	for (int i = 1; i < 16; i++)
	{
		if (i != 8 && i != leftCornerBallNum && i != rightCornerBallNum)
			arrangeableBalls.Add(i);
	}
	for (int i = arrangeableBalls.Num() - 1; i > 1; i--)
	{
		int randIdx = FMath::RandRange(0, i);
		int tmp = arrangeableBalls[i];
		arrangeableBalls[i] = arrangeableBalls[randIdx];
		arrangeableBalls[randIdx] = tmp;
	}
	arrangeableBalls.Insert(8, 4);
	arrangeableBalls.Insert(leftCornerBallNum, 10);
	arrangeableBalls.Insert(rightCornerBallNum, 14);
	FVector v1 = GetFoulBallDefaultReturnLocationOnTable() - GetCueBallDefaultLocationOnTable();
	v1.Normalize();
	v1 *= 100.0f * objects->balls[0]->scale;
	FVector v2 = v1;
	v1 = v1.RotateAngleAxis(-30.0f, FVector(0.0f, 0.0f, 1.0f));
	v2 = v2.RotateAngleAxis(30.0f, FVector(0.0f, 0.0f, 1.0f));
	// triangular numbers maffs starts here
	for (int i = 0; i < 15; i++)
	{
		int row = int((sqrt(1.0f + 8.0f * i) - 1.0f) / 2.0f);
		int v1Steps = (row * (row + 3)) / 2 - i;
		int v2Steps = i - (row * (row + 1)) / 2;
		FVector loc = GetFoulBallDefaultReturnLocationOnTable() + FVector(0.0f, 0.0f, 50.0f * objects->balls[0]->scale);
		loc += v1 * v1Steps + v2 * v2Steps;
		FRotator rot = FRotator(FMath::RandRange(0.0f, 360.0f),
			FMath::RandRange(0.0f, 360.0f),
			FMath::RandRange(0.0f, 360.0f));
        objects->physicsEngine->MoveBallAndStop(arrangeableBalls[i], loc, rot);
		//balls[arrangeableBalls[i]]->mesh->SetWorldLocation(loc, false, nullptr, ETeleportType::TeleportPhysics);
		//balls[arrangeableBalls[i]]->mesh->SetWorldRotation(rot, false, nullptr, ETeleportType::TeleportPhysics);
		//InfoUpload_LoadBallTransform(arrangeableBalls[i], loc, rot, false);
	}
    ReturnCueBall();
	/*InfoUpload_LoadBallTransform(0,
		GetCueBallDefaultLocationOnTable() + FVector(0.0f, 0.0f, 50.0f * objects->balls[0]->scale),
		FRotator(0.0f), 
		false, 
		true, 
		true,
		false);*/
}

std::function<bool(FVector2D)> APoolGameStateBase::GetRulesetFunction ()
{
    switch (rules)
    {
        case ShotRuleset::RULES_ANYWHERE:
            return nullptr;
        case ShotRuleset::RULES_BEHIND_HEADSTRING:
            return [](FVector2D pos) {return pos.Y < 0.5f;};
        case ShotRuleset::RULES_KITCHEN:
            return [](FVector2D pos) {return pos.Y < 0.5f &&
                                            (pos.X - 0.5f)*(pos.X - 0.5f) +
                (pos.Y - 0.5f)*(pos.Y - 0.5f) < 0.125f;};
        case ShotRuleset::RULES_CIRCLE_OF_BALLS:
            return [](FVector2D pos) {return (pos.X - 0.5f)*(pos.X - 0.5f) +
                (pos.Y - 1.0f)*(pos.Y - 1.0f) < CIRCLE_OF_BALLS_RADIUS*CIRCLE_OF_BALLS_RADIUS;};
    }
	return nullptr;
}

void APoolGameStateBase::SetRuleset(ShotRuleset rules_)
{
    if (!HasAuthority())
        return;
    rules = rules_;
    UploadRules(rules);
}

void APoolGameStateBase::UploadRules_Implementation(ShotRuleset rules_)
{
    if (HasAuthority())
        return;
    rules = rules_;
}

void APoolGameStateBase::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APoolGameStateBase,FirstPlayerPS)
	DOREPLIFETIME(APoolGameStateBase,SecondPlayerPS)
	DOREPLIFETIME(APoolGameStateBase,SpectatorCount)
	DOREPLIFETIME(APoolGameStateBase,GameTime)
}