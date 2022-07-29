// Fill out your copyright notice in the Description page of Project Settings.


#include "BallRestLocation.h"

// Sets default values
ABallRestLocation::ABallRestLocation() : 
	root (CreateDefaultSubobject<USceneComponent>(TEXT("RootComp")))
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ABallRestLocation::BeginPlay()
{
	Super::BeginPlay();
}

FVector ABallRestLocation::GetLocation()
{
	return GetActorLocation();

}
