// Fill out your copyright notice in the Description page of Project Settings.


#include "TableWalls.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATableWalls::ATableWalls() :
    root      (nullptr),
    wallsMesh (nullptr)
{
	PrimaryActorTick.bCanEverTick = false;
    
    root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    RootComponent = root;
    wallsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallsMesh"));
    wallsMesh->SetCollisionProfileName(TEXT("OverlapAll"));
    wallsMesh->SetVisibility(false, false);
}

// Called when the game starts or when spawned
void ATableWalls::BeginPlay()
{
	Super::BeginPlay();
	
}
