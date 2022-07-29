// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"
#include "Table.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "SharedDefs.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "PoolGameStateBase.h"
#include "ObjectGatherer.h"


ABall::ABall():
    objects                    (nullptr),
    //linearVelocitySnapshot     (),
    //angularVelocitySnapshot    (),
    //worldLocationSnapshot      (),
    //snapshotIdx                (0),
	//isMoving                   (true),
    //lastHitTime                (),
    pocketed                   (false),
	scale                      (0.06f),
    //lastCollided               (nullptr),
    //lastCollisionTime          (),
    //collisionDelay             (),
    //lastTableCollisionTime     (),
	planeMesh                  (nullptr),
	shadowSpringArm            (nullptr),
	shadowMaterial             (nullptr),
	shadowZdisplacement        (scale * 50.0f * 0.6f),
	shadows                    ()//,
    //totalVelocityMovingAverage (0.0f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ball mesh"));
	mesh->SetupAttachment(root);
    //mesh->SetupAttachment(root);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	mesh->SetStaticMesh(SphereMeshAsset.Object);
	mesh->SetWorldScale3D(FVector(scale));
    mesh->SetCollisionProfileName(TEXT("BallPreset"));
    //mesh->OnComponentBeginOverlap.AddDynamic(this, &ABall::OverlapBegin);
    //mesh->SetGenerateOverlapEvents(false);

	//mesh->SetAngularDamping(0.5);
	//mesh->SetLinearDamping(0.5);
	//mesh->AttachTo(RootComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));
	planeMesh = PlaneMeshAsset.Object;
	static ConstructorHelpers::FObjectFinder<UMaterial> ShadowMaterial(TEXT("/Game/ASSETS/Balls/BallShadowMaterial.BallShadowMaterial"));
	shadowMaterial = ShadowMaterial.Object;

	shadowSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Shadow spring arm"));
	shadowSpringArm->bInheritPitch = false;
	shadowSpringArm->bInheritRoll  = false;
	shadowSpringArm->bInheritYaw   = false;
	shadowSpringArm->TargetArmLength = 0.0f;
	shadowSpringArm->bDoCollisionTest = false;
	shadowSpringArm->SetupAttachment(root);
	//mesh->SetAllUseCCD(true);

}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();
    GATHER_OBJECT_GATHERER(objects)
	scale = mesh->GetComponentScale().Size() / sqrt(3.0f);
    shadowZdisplacement = scale * 50.0f * 45.0f/ 60.0f;
	mesh->SetNotifyRigidBodyCollision(true);
	pocketed = false;
	mesh->SetSimulatePhysics(false);
	//lastCollided = nullptr;
	//lastCollisionTime = GetGameTimeSinceCreation();
	//lastTableCollisionTime = GetGameTimeSinceCreation();
	//collisionDelay = 0.1;
	UpdateOverlaps(true);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), 
		                                  APoolLight::StaticClass(), 
		                                  FoundActors); 
	
	for (auto& foundLight : FoundActors)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Found light"));
		UStaticMeshComponent* newShadowMesh = NewObject<UStaticMeshComponent>(shadowSpringArm);
		//if (!newShadowMesh) GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Mesh not created"));
		newShadowMesh->SetStaticMesh(planeMesh);
		newShadowMesh->SetMaterial(0, shadowMaterial);
		newShadowMesh->SetupAttachment(shadowSpringArm);
		newShadowMesh->SetCollisionProfileName(TEXT("NoCollision"));
		newShadowMesh->RegisterComponent();
		UShadowInfo si = { newShadowMesh , (APointLight*)foundLight };
		shadows.Push(si);
		//shadowMesh = newShadowMesh;
		UpdateShadow(si);
	}

	//GEngine->AddOnScreenDebugMesdsage(-1, 15.0f, FColor::Green, FVector(isMoving).ToString());
}

void ABall::UpdateShadow(UShadowInfo& si)
{
    
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, TEXT("Updating"));
    float ballRadius = scale * 50.0f;
    FVector ballLocation = /*GetActorLocation();*/objects->physicsEngine->GetBallLocation(ballNum);
	FVector lightRay = ballLocation - si.source->GetActorLocation();
	lightRay.Normalize();
	float normalAngleCos = -FVector::DotProduct(lightRay, FVector(0.0f, 0.0f, 1.0f));
	float normalAngleTan = sqrt(1.0f / (normalAngleCos * normalAngleCos) - 1.0f);
	FVector2D flatLightVector = FVector2D(lightRay.X, lightRay.Y);
	flatLightVector.Normalize();
	flatLightVector *= ballRadius * normalAngleTan;

	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::White, flatLightVector.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, lightRay.ToString());
	FVector newShadowCenterWorld = ballLocation + FVector(flatLightVector.X, flatLightVector.Y, -shadowZdisplacement * scale / 0.06f);
	float normalAngleSin = normalAngleTan * normalAngleCos;
	float otherSideDisplacement = ballRadius * (1.0f - normalAngleSin) / normalAngleCos;
	float shadowEllipseLongRadius = flatLightVector.Size() + otherSideDisplacement;

	float shadowScaleToSizeCoefficient = 1.0f / ballRadius;
	si.shadowMesh->SetWorldLocation(newShadowCenterWorld);
	//si.shadowMesh->SetRelativeLocation(FVector(0.0f));
	si.shadowMesh->SetRelativeScale3D(FVector(shadowEllipseLongRadius / ballRadius,
		                                   1.1f, 1.0f) * FVector(scale, scale, 1.0f));
	float rotationAngleTan = flatLightVector.X / flatLightVector.Y;
	FVector newShadowCenterWorldAtBallHeight = newShadowCenterWorld;
	newShadowCenterWorldAtBallHeight.Z = ballLocation.Z;
	FRotator look = UKismetMathLibrary::FindLookAtRotation(newShadowCenterWorldAtBallHeight, ballLocation);
	//si.shadowMesh->SetWorldRotation(look);
}

void ABall::UpdateShadows()
{
    if (!objects->gi)
        return;
    if (!objects->gi->data->settings->GetDrawBallsShadows())
    {
        for (auto& si : shadows)
            si.shadowMesh->SetVisibility(false, false);
    }
    else
    {
        for (auto& si : shadows)
        {
            si.shadowMesh->SetVisibility(true, false);
            UpdateShadow(si);
        }
    }
    
    
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
    UpdateShadows();
    
    //NewSnapshot();
    
	/*if (GetGameTimeSinceCreation() - lastHitTime < objects->settings->impulsePropagationDelay) return;
	float currentVelocity = this->GetVelocitySnap().Size();
    float currentAngularVelocity = this->GetAngularVelocitySnap().Size();
    
    float newVelocityMovingAverageInput = (currentVelocity + currentAngularVelocity * scale * 50.0f)/2.0f;
    if (totalVelocityMovingAverage < objects->settings->stopVelocity && newVelocityMovingAverageInput > objects->settings->stopVelocity)
        totalVelocityMovingAverage = newVelocityMovingAverageInput;
    else
        totalVelocityMovingAverage = totalVelocityMovingAverage * 0.9f + newVelocityMovingAverageInput * 0.1f;
    
	if (totalVelocityMovingAverage < objects->settings->stopVelocity)
	{
        mesh->SetPhysicsLinearVelocity(FVector(0.0f));
        mesh->SetPhysicsAngularVelocityInDegrees(FVector(0.0f));
        if (isMoving)
        {
            //PRINTF((TEXT("Stopping ball %d"), ballNum), Red)
            totalVelocityMovingAverage = 0.0f;
            isMoving = false;
            GAME_STATE_SYNC(gs, BallStopped(ballNum))
        }
	}
    else
	if (currentVelocity < objects->settings->startDecelerationVelocity)
	{
		mesh->SetLinearDamping(objects->settings->slowingLinearDamping);
		mesh->SetAngularDamping(objects->settings->slowingAngularDamping);
	}
    else
	{
		isMoving = true;
		GAME_STATE_SYNC(gs, BallMoved(ballNum))
		mesh->SetLinearDamping(objects->settings->defaultLinearDamping);
		mesh->SetAngularDamping(objects->settings->defaultAngularDamping);
	}*/

}


void ABall::Push(FVector impulse, FVector location)
{
	//mesh->AddImpulseAtLocation(impulse, location);
	//lastHitTime = GetGameTimeSinceCreation();
	//isMoving = true;
    objects->shotinfo->BallMoved(ballNum);
}

void ABall::PushVelocity_Implementation(FVector vel, bool isTable)
{
	////123 PRINTF((TEXT(__FUNCSIG__ " hasAuth: %d"), HasAuthority()), Green)
	//PushVelocity_Multicast_Implementation(vel);
//PushVelocity_Server(vel, isTable);
	//if (!HasAuthority())
	//{
		//dynamic_cast<APoolPlayerControllerBase*>(UGameplayStatics::GetPlayerController(GetWorld(), 0))->PushBall(this, vel, isTable);
	//}
	//else
	{
		PushVelocity_Multicast_Implementation(vel, isTable);
	}
}
void ABall::PushVelocity_Server_Implementation(FVector vel, bool isTable)
{
	////123 PRINTF((TEXT(__FUNCSIG__ " hasAuth: %d"), HasAuthority()), Green)
		PushVelocity_Multicast(vel);
}
void ABall::PushVelocity_Multicast_Implementation(FVector vel, bool isTable)
{
	////123 PRINTF((TEXT(__FUNCSIG__ " hasAuth: %d"), HasAuthority()), Green)
	//mesh->SetSimulatePhysics(true);
	//vel.Z = 0.0f;
    objects->physicsEngine->ApplyLinearVelocity(ballNum, vel);
	//mesh->SetPhysicsLinearVelocity(vel);
	//lastHitTime = GetGameTimeSinceCreation();
	//oldVelocity = vel;
	//isMoving = true;
    objects->shotinfo->BallMoved(ballNum);
	//vel.Normalize();
	////123 PRINTF((TEXT("Pushed, vel = %.5f %.5f"), vel.X, vel.Y), White)

	/*if (isTable)
	{
		lastTableCollisionTime = GetGameTimeSinceCreation();
	}*/
}

bool ABall::CanCollideWithTable()
{
	if (pocketed) return false;
	return true;
}

bool& ABall::Pocketed()
{
	return pocketed;
}
/*void ABall::NotifyHit(UPrimitiveComponent* MyComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	bool bSelfMoved,
	FVector HitLocation,
	FVector HitNormal,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	ABall* that = dynamic_cast<ABall*> (Other);


	float passedTime = GetGameTimeSinceCreation() - lastCollisionTime;
	if (!that || (lastCollided == that && collisionDelay > passedTime)) return;

	//if (that->ballNum == 0)
	//	dynamic_cast<ACueBall*> (that)->GotHit();
	GAME_STATE_SYNC(gs, AddToHitLog(ballNum, that->ballNum))

	FVector centers = this->mesh->GetComponentLocation() - that->mesh->GetComponentLocation();
    FVector thisVel = this->GetVelocitySnap(true);
    FVector thatVel = that->GetVelocitySnap(true);
	FVector velRel  = thisVel - thatVel;
    
    if ( FVector::DotProduct(centers, velRel) > 1.0f && (velRel.Size() > scale * 50.0f / 10.0f) && (centers.Size() > scale * 50.0f * 2 * 1.5f))
    {
        this->RevertToPrevSnap();
        that->RevertToPrevSnap();
        PRINTF((TEXT("Found fake collision between %d and %d"), this->ballNum, that->ballNum), Red)
        return;
    }
    centers.Normalize();

	FVector velNorm = FVector::DotProduct(velRel, centers) * centers;
	FVector thisNewVelocity = thisVel - velNorm;
	FVector thatNewVelocity = thatVel + velNorm;


	FVector thisAngular = this->GetAngularVelocitySnap(true);
	FVector thatAngular = that->GetAngularVelocitySnap(true);
	FVector thisR = HitLocation - this->mesh->GetComponentLocation();
	FVector thatR = HitLocation - that->mesh->GetComponentLocation();

	FVector thisVelocityAtHitLocation = this->GetVelocitySnap(true) - this->GetVelocitySnap(true).ProjectOnToNormal(centers) + FVector::CrossProduct(thisAngular, thisR);
	FVector thatVelocityAtHitLocation = that->GetVelocitySnap(true) - that->GetVelocitySnap(true).ProjectOnToNormal(centers) + FVector::CrossProduct(thatAngular, thatR);

	FVector collisionPointsRelativeVelocity = thisVelocityAtHitLocation - thatVelocityAtHitLocation;
	float relativeVelocityCoeff = 0.0f;
	if (velRel.Size() < 50.0) relativeVelocityCoeff = 1.0f;
	else relativeVelocityCoeff = 50.0 / velRel.Size();
	float inducedThrowCoeff = 0.1f * relativeVelocityCoeff;
	FVector inducedThrow = collisionPointsRelativeVelocity * inducedThrowCoeff;
	thatNewVelocity += inducedThrow;
	thisNewVelocity -= inducedThrow;

	float inducedSpinCoeff = 0.1f * relativeVelocityCoeff;
	FVector thisInducedSpin = FVector::CrossProduct(thisR, collisionPointsRelativeVelocity) * inducedSpinCoeff / thisR.SizeSquared();
	FVector thatInducedSpin = FVector::CrossProduct(thatR, collisionPointsRelativeVelocity) * inducedSpinCoeff / thatR.SizeSquared();
	thisAngular -= thisInducedSpin;
	thatAngular += thatInducedSpin;
	this->mesh->SetPhysicsAngularVelocityInRadians(thisAngular);
	that->mesh->SetPhysicsAngularVelocityInRadians(thatAngular);
    this->NewSnapshot();
    that->NewSnapshot();

	this->PushVelocity(thisNewVelocity);
	that->PushVelocity(thatNewVelocity);
	lastCollided = that;
	that->lastCollided = this;
	that->lastCollisionTime = that->GetGameTimeSinceCreation();
	lastCollisionTime = GetGameTimeSinceCreation();
}*/
ABall::~ABall() {}


void ABall::AfterPocketed()
{
	if (!pocketed) return;
    objects->shotinfo->BallPocketed(ballNum);
	/*mesh->SetPhysicsLinearVelocity(FVector(0.0f));
	mesh->SetPhysicsAngularVelocityInRadians(FVector(0.0f));
	FVector hbsl = gs->GetPocketedBallStorageLocation(ballNum);
	mesh->SetSimulatePhysics(false);
	mesh->SetEnableGravity(false);
	//mesh->SetCollisionProfileName(TEXT("NoCollision"));
	mesh->SetWorldLocation(hbsl, false, nullptr, ETeleportType::TeleportPhysics);
	mesh->SetWorldRotation(FRotator(0.0f, 90.0f, 0.0f));*/
}

void ABall::AfterKnocked()
{
    objects->shotinfo->BallKnockedOff(ballNum);

	/*FVector hbsl = gs->GetPocketedBallStorageLocation(ballNum);
	mesh->SetSimulatePhysics(false);
	mesh->SetEnableGravity(false);
	//mesh->SetCollisionProfileName(TEXT("NoCollision"));
	mesh->SetWorldLocation(hbsl, false, nullptr, ETeleportType::TeleportPhysics);
	mesh->SetWorldRotation(FRotator(0.0f, 90.0f, 0.0f));*/

	/*if (ballNum == 0)
		gs->ReturnCueBall();
	else 
		gs->ReturnFoulBall(ballNum);*/
}

/*void ABall::NewSnapshot()
{
    snapshotIdx++;
    snapshotIdx %= 2;
    linearVelocitySnapshot[snapshotIdx] = mesh->GetPhysicsLinearVelocity();
    angularVelocitySnapshot[snapshotIdx] = mesh->GetPhysicsAngularVelocityInRadians();
    worldLocationSnapshot[snapshotIdx] = mesh->GetComponentLocation();
}


FVector& ABall::GetVelocitySnap(bool prev)
{
    return linearVelocitySnapshot[prev ? (1 - snapshotIdx) : snapshotIdx];
}
FVector& ABall::GetAngularVelocitySnap(bool prev)
{
    return angularVelocitySnapshot[prev ? (1 - snapshotIdx) : snapshotIdx];
}
FVector& ABall::GetWorldLocationSnap(bool prev)
{
    return worldLocationSnapshot[prev ? (1 - snapshotIdx) : snapshotIdx];
}

void ABall::RevertToPrevSnap()
{
    mesh->SetPhysicsLinearVelocity(linearVelocitySnapshot[1 - snapshotIdx]);
    mesh->SetPhysicsAngularVelocityInRadians(angularVelocitySnapshot[1 - snapshotIdx]);
    mesh->SetWorldLocation(worldLocationSnapshot[1 - snapshotIdx]);
    snapshotIdx++;
    snapshotIdx %= 2;
}*/


//
//void ABall::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps)
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(ABall, isMoving);
//}
