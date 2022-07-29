// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PhysicsSetup.h"

#include "CoreMinimal.h"
#include "Engine.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "PoolLight.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
//#include <Runtime\Engine\Public\Net\UnrealNetwork.h>
#include "Ball.generated.h"

class AObjectGatherer;

struct UShadowInfo
{
	UStaticMeshComponent* shadowMesh;
	APointLight* source;
};

UCLASS()
class SEFAPOOL_API ABall : public AActor
{
	GENERATED_BODY()
public:
	AObjectGatherer* objects;
    
    UPROPERTY(VisibleAnywhere)
    USceneComponent* root;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ball settings")
	uint8 ballNum;
    
    //FVector linearVelocitySnapshot[2];
    //FVector angularVelocitySnapshot[2];
    //FVector worldLocationSnapshot[2];
    //int snapshotIdx;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* mesh;
	//bool isMoving;
	//float lastHitTime;
	bool pocketed;
public:
	float scale;
protected:
	//ABall* lastCollided;
	//float lastCollisionTime;
	//float collisionDelay;
	//float lastTableCollisionTime;
	UStaticMesh* planeMesh;
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* shadowSpringArm;
	UMaterial* shadowMaterial;
	float shadowZdisplacement;
	TArray<UShadowInfo> shadows;
    //float totalVelocityMovingAverage;


public:
	ABall();

	virtual void BeginPlay() override;
	void UpdateShadow(UShadowInfo& si);
    void UpdateShadows();
	virtual void Tick(float DeltaTime) override;
	virtual void Push(FVector impulse, FVector location);
	UFUNCTION(Client, Reliable)
	void PushVelocity(FVector vel, bool isTable = false);
	void PushVelocity_Implementation(FVector vel, bool isTable = false);
	UFUNCTION(Server, Reliable)
	void PushVelocity_Server(FVector vel, bool isTable = false);
	void PushVelocity_Server_Implementation(FVector vel, bool isTable = false);
	UFUNCTION(NetMulticast, Reliable)
	void PushVelocity_Multicast(FVector vel, bool isTable = false);
	void PushVelocity_Multicast_Implementation(FVector vel, bool isTable = false);
	bool& Pocketed();
	/*virtual void NotifyHit(UPrimitiveComponent* MyComp,
				   AActor* Other,
				   UPrimitiveComponent* OtherComp,
				   bool bSelfMoved,
				   FVector HitLocation,
				   FVector HitNormal,
				   FVector NormalImpulse,
				   const FHitResult& Hit);*/
	virtual ~ABall();
	virtual void AfterPocketed();
	virtual void AfterKnocked();
	virtual bool CanCollideWithTable();
	//void NewSnapshot();
    //FVector& GetVelocitySnap(bool prev = false);
    //FVector& GetAngularVelocitySnap(bool prev = false);
    //FVector& GetWorldLocationSnap(bool prev = false);
    //void RevertToPrevSnap();
	//virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) override;
};
