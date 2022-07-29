// Fill out your copyright notice in the Description page of Project Settings.


#include "Table.h"
#include "UObject/ConstructorHelpers.h"
#include "PoolGameStateBase.h"
#include "Engine/StaticMesh.h"
#include "Components/SplineComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ATable::ATable() :
    clothMaterials (),
	physicsSetup(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("table"));
	mesh->SetWorldScale3D(FVector(1.25));

	secMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("tableSec"));
	secMesh->SetCollisionProfileName(TEXT("BlockAllDynamic")); 
	secMesh->SetupAttachment(mesh);
	spectatorCameraTarget = CreateDefaultSubobject<USceneComponent>(TEXT("SpectatorCameraTargetComp"));
	defaultCueBallPos = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultCueBallPosComp"));
	defaultFoulPos = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultFoulPosComp"));
    cushionHeight = CreateDefaultSubobject<USceneComponent>(TEXT("cushionheight"));
    cushionWidth1 = CreateDefaultSubobject<USceneComponent>(TEXT("cushionWidth1"));
    cushionWidth2 = CreateDefaultSubobject<USceneComponent>(TEXT("cushionWidth2"));
	TableViewSpline = CreateDefaultSubobject<USplineComponent>("TableSpline");
    
    
#define LOAD_CLOTH_TEXTURE(name, fileName)\
ConstructorHelpers::FObjectFinder<UMaterial> name##Material (TEXT("/Game/ASSETS/TableCloth/M_" #fileName ".M_" #fileName));\
    clothMaterials.Add(name##Material.Object);
    
    LOAD_CLOTH_TEXTURE(BLEU_TOURNAMENT, BleuTournament)
    LOAD_CLOTH_TEXTURE(BLEU_ELECTRIQUE, BleuElectrique)
    LOAD_CLOTH_TEXTURE(VERT_EMPIRE, VertEmpire)
    LOAD_CLOTH_TEXTURE(VERT_JAUNE, VertJaune)
    LOAD_CLOTH_TEXTURE(BORDEAUX, Bordeaux)
    LOAD_CLOTH_TEXTURE(BW_COLORABLE, Colorable)
    
}

// Called when the game starts or when spawned
void ATable::BeginPlay()
{
	Super::BeginPlay();
	mesh->SetNotifyRigidBodyCollision(true);
	mesh->SetSimulatePhysics(false);
	secMesh->SetNotifyRigidBodyCollision(false);
	secMesh->SetSimulatePhysics(false);
	UpdateOverlaps(true);
    
    colorableMaterial = UMaterialInstanceDynamic::Create(clothMaterials[BW_COLORABLE], this);
}

// Called every frame
/*
void ATable::NotifyHit(UPrimitiveComponent* MyComp,
                       AActor* Other,
                       UPrimitiveComponent* OtherComp,
	                   bool bSelfMoved,
	                   FVector HitLocation,
	                   FVector HitNormal,
	                   FVector NormalImpulse,
	                   const FHitResult& Hit)
{
	ABall* ball = dynamic_cast<ABall*> (Other);
	if (!ball) return;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, ball->GetName());
	if (!ball->CanCollideWithTable()) return;
	GAME_STATE_SYNC(gs, AddToHitLog(ball->ballNum, SIDERAIL_HIT))
	if (ball->GetVelocitySnap().Z > 0.0f)
        ball->GetVelocitySnap().Z *= 0.5f;
	ball->PushVelocity(ball->GetVelocitySnap(), true); //used to register table hit
	//if (fabs(HitNormal.X) > fabs(HitNormal.Y) && fabs(HitNormal.X / (HitNormal.Y + 1e-6f)) < 50.0f) return;
	//if (fabs(HitNormal.Y) > fabs(HitNormal.X) && fabs(HitNormal.Y / (HitNormal.X + 1e-6f)) < 50.0f) return;
	FVector velocity = ball->GetVelocitySnap(true);
	FVector normal = velocity.ProjectOnToNormal(HitNormal);
	FVector tangent = velocity - normal;
	velocity = tangent - physicsSetup->normalVelocityLoss * normal;
	//if (velocity.Z > 0.0f)
	//	velocity.Z *= 0.1;
	FVector angular = ball->GetAngularVelocitySnap(true);
	FVector r = HitLocation - ball->GetWorldLocationSnap();
	//angular.X *= 0.1f;
	//angular.Y *= 0.1f;

	FVector tangentVelocityAtHitLocation = tangent + FVector::CrossProduct(angular, r);
	//OtherComp->SetPhysicsLinearVelocity(velocity);
	//float spinEffectCoeff = 0.0f;
	//const float spinVelocityThreshold = 700.0;
	//if (normal.Size() >= spinVelocityThreshold) spinEffectCoeff = 1.0;
	//else spinEffectCoeff = normal.Size() / spinVelocityThreshold;
	//spinEffectCoeff = pow(spinEffectCoeff, 2.5);
    float spinEffectCoeff = 0.1f;

	velocity -= tangentVelocityAtHitLocation * spinEffectCoeff;
	velocity.Z *= 0.5f;
	ball->PushVelocity(velocity, true);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, (tangentVelocityAtHitLocation * spinEffectCoeff).ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FVector(spinEffectCoeff).ToString());

	//if (ball->ballNum == 0)
	//	dynamic_cast<ACueBall*> (ball)->GotHit();
}

void ATable::PocketOverlapBegin(UPrimitiveComponent* MyComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABall* ball = dynamic_cast<ABall*> (Other);
	if (!ball) return;
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, ball->GetName());
	if (ball->Pocketed()) return;
	ball->Pocketed() = true;
	FVector velocity = ball->GetVelocitySnap();
	velocity.X *= 0.5f;
	velocity.Y *= 0.5f;
	OtherComp->SetPhysicsLinearVelocity(velocity);
    PRINTF((TEXT("ball %d started pocketing"), ball->ballNum), Green)
	return;
}

void ATable::PocketOverlapEnd(UPrimitiveComponent* MyComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ABall* ball = dynamic_cast<ABall*> (Other);
	if (!ball) return;
	if (!ball->Pocketed()) return;
    
    PRINTF((TEXT("ball %d ended pocketing"), ball->ballNum), Green)
	ball->AfterPocketed();
	return;
}

void ATable::WallOverlapBegin(UPrimitiveComponent* MyComp,
	AActor* Other,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABall* ball = dynamic_cast<ABall*> (Other);
	if (!ball) return;
	ball->AfterKnocked();
	//GAME_STATE_SYNC(gs, BallKnockedOff(ball->ballNum))
    PRINTF((TEXT("ball %d knocked off"), ball->ballNum), Green)
	return;
}*/

FVector ATable::GetCueBallDefaultPos()
{
	return defaultCueBallPos->GetComponentLocation();
}

FVector ATable::GetFoulDefaultPos()
{
	return defaultFoulPos->GetComponentLocation();
}

ATable::~ATable() {}


void ATable::ApplyMaterial(E_TableClothColors color)
{
    if (color < 0 || color > BW_COLORABLE) return;
    BPApplyMaterial(clothMaterials[color]);
}

void ATable::ApplyColor(FLinearColor color, float adj)
{
    colorableMaterial->SetVectorParameterValue(TEXT("BlendableColor"), color);
    colorableMaterial->SetScalarParameterValue(TEXT("Adjustment"), adj);
    BPApplyDynMaterial(colorableMaterial);
}

UMaterialInstanceDynamic* ATable::GetMaterialInstance()
{
    return colorableMaterial;
}
