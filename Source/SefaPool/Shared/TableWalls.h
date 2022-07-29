// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine.h"
#include "Math/Vector.h"
#include "Components/SceneComponent.h"
#include "TableWalls.generated.h"

UCLASS()
class SEFAPOOL_API ATableWalls : public AActor
{
	GENERATED_BODY()
    
    UPROPERTY(VisibleAnywhere)
    USceneComponent* root;
    
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* wallsMesh;
    
public:	
	// Sets default values for this actor's properties
	ATableWalls();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

};
