// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BallRestLocation.generated.h"

// Place this where you want balls stored in room after being pocketed. Exact location = where the 0 ball touches

UCLASS()
class SEFAPOOL_API ABallRestLocation : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	USceneComponent* root;
public:	
	// Sets default values for this actor's properties
	ABallRestLocation(); 
	FVector GetLocation();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


};
