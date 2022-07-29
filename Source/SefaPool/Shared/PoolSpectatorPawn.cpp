// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolSpectatorPawn.h"
#include "Table.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "SharedDefs.h"
#include "ObjectGatherer.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
APoolSpectatorPawn::APoolSpectatorPawn() :
    objects          (nullptr),
	root             (CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"))),
	camera           (CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"))),
    defaultSplineLoc (0.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	camera->SetupAttachment(root);
	camera->SetRelativeLocation(FVector(0.0f));
	camera->SetRelativeLocation(FVector(0.0f));

}

void APoolSpectatorPawn::BeginPlay()
{
	Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects)
	ResetCam();
}

void APoolSpectatorPawn::ResetCam()
{
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi) return;
    auto spline = objects->table->TableViewSpline;
    //defaultSplineLoc = objects->settings->defaultSplineLoc;
    defaultSplineLoc = objects->gi->data->settings->GetSpectatingCamPosition();
	FVector loc = spline->GetLocationAtDistanceAlongSpline(spline->GetSplineLength() * defaultSplineLoc, ESplineCoordinateSpace::Type::World);
    loc += FVector(0.0f, 0.0f, objects->gi->data->settings->GetSpectatingCamHeight());
	camera->SetWorldLocation(loc);
    camera->SetFieldOfView(objects->gi->data->settings->GetSpectatingCamFOV());
	AdjustLook(loc);
}

void APoolSpectatorPawn::AdjustLook(FVector loc)
{
    UPoolServerGameInstance* gi = dynamic_cast<UPoolServerGameInstance*> (UGameplayStatics::GetGameInstance(GetWorld()));
    if (gi) return;
    FVector lookTarget = objects->table->spectatorCameraTarget->GetComponentLocation();
    lookTarget.Z = objects->gi->data->settings->GetSpectatingCamCenteringHeight();
	FRotator look = UKismetMathLibrary::FindLookAtRotation(loc, lookTarget);
	camera->SetWorldRotation(look);
}

