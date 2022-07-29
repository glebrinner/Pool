// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsSetup.generated.h"

UCLASS()
class SEFAPOOL_API APhysicsSetup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APhysicsSetup();

	// Physical properties of balls. The idea is to change this properties in the game dynamically.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float startDecelerationVelocity; // 1.0 - 50.0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float stopVelocity; // 0.001 - 0.5
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float defaultAngularDamping; // 0.1 - 0.8
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float slowingAngularDamping; // 0.3 - 1.0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float defaultLinearDamping; // 0.1 - 0.8
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float slowingLinearDamping; // 0.3 - 1.0
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float impulsePropagationDelay; // 0.05 - 0.5
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
        float orthoCamMovementTableFraction; 

	// Physical properies of table.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Physics settings")
		float normalVelocityLoss; // 0.5 - 1.0

	// Properties of Drag sensitivity
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float xTouchSensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float yTouchSensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int xAxisOrientation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int yAxisOrientation;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float defaultSplineLoc;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
