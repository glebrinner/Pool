// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolTableCameras.h"
#include "SharedDefs.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "ObjectGatherer.h"

// Sets default values
APoolTableCameras::APoolTableCameras() : 
	pocketCenters (),
	pocketCameras (),
	table         (nullptr),
	tableLen      (0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void APoolTableCameras::BeginPlay()
{
	if (!this)
		ERROR_DEBUG_PRINT_FUNC
	Super::BeginPlay();
    
    GATHER_OBJECT_GATHERER(objects)
    
    table = objects->table;
    ATablePockets* tablePockets = objects->tablePockets;
	SetActorLocation(table->mesh->GetComponentLocation());
    
#define POCKET_ZONE(n)\
    FVector2D pocket_##n;\
    FVector pocket_##n##_3D = tablePockets->pocket_##n->GetComponentLocation(); \
    pocket_##n.X = pocket_##n##_3D.X; \
    pocket_##n.Y = pocket_##n##_3D.Y; \
    pocketCenters.Add(pocket_##n);

	POCKET_ZONE(0)
	POCKET_ZONE(1)
	POCKET_ZONE(2)
	POCKET_ZONE(3)
	POCKET_ZONE(4)
	POCKET_ZONE(5)
#undef POCKET_ZONE

	FVector t1 = table->defaultCueBallPos->GetComponentLocation();
	FVector t2 = table->defaultFoulPos->GetComponentLocation();
	FVector halfTable = t1 - t2;
	tableLen = 2.0f * halfTable.Size();
	for (auto pock : pocketCenters)
	{
		UCameraComponent* cc = NewObject<UCameraComponent>(this);
		FVector loc(pock, (t1.Z + t2.Z)/2.0f);
		FVector shift = FVector(0.0f, 0.0f, tableLen / 6.0f);
		float collinearity = FVector::DotProduct(loc - t1, loc - t2);
		FVector hor_shift = loc - (t1 + t2) / 2.0f;
		hor_shift.Z = 0.0f;
		hor_shift.Normalize();
		hor_shift *= tableLen / 4.0f;
		shift += hor_shift;
		cc->SetWorldLocation(loc + shift);
		pocketCameras.Add(cc);
	}

}


bool APoolTableCameras::ProcessShot(ABall* targetBall, FVector targetVector)
{
	//PRINTF((TEXT("Hello there")), White);

	FHitResult hitResult = {};
	const FVector& ballPos = targetBall->mesh->GetComponentLocation();
	FCollisionQueryParams params(FName(TEXT("PoolTableCamersCollisionTrace")), true);
	params.bIgnoreTouches = false;
	FCollisionResponseParams params1 = {};
	bool hasHit = GetWorld()->LineTraceSingleByChannel(hitResult,
		ballPos,
		ballPos + tableLen * targetVector,
		ECC_WorldDynamic,
		params,
		params1);
	if (!hasHit || hitResult.Actor.Get() != table) return false;


	for (int i = 0; i < pocketCenters.Num(); i++)
	{
		FVector pocketPosAtSameHeight(pocketCenters[i], hitResult.ImpactPoint.Z);
		if ((pocketPosAtSameHeight - hitResult.ImpactPoint).Size() > tableLen / 10.0f)
			continue;
		//PRINTF((TEXT("found pocket %d"), i), Blue);
		for (auto cc : pocketCameras) cc->SetActive(false);
		pocketCameras[i]->SetActive(true);
		/*FVector lookTarget = (ballPos + hitResult.ImpactPoint) / 2.0f;
		FRotator look = UKismetMathLibrary::FindLookAtRotation(pocketCameras[i]->GetComponentLocation(), lookTarget);
		pocketCameras[i]->SetWorldRotation(look);*/
        FRotator look = UKismetMathLibrary::FindLookAtRotation(pocketCameras[i]->GetComponentLocation(), objects->physicsEngine->GetTableCenter());
        pocketCameras[i]->SetWorldRotation(look);
		return true;
	}
	return false;
}

// Called every frame
void APoolTableCameras::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

