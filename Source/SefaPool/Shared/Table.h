// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine.h"
#include "GameFramework/Actor.h"
#include "Math/Vector.h"
#include "Camera/CameraComponent.h"
#include "Components/SceneComponent.h"
#include "PhysicsSetup.h"
#include "Materials/Material.h"
#include "Table.generated.h"

class USplineComponent;


UENUM(BlueprintType)
enum E_TableClothColors
{
    BLEU_TOURNAMENT = 0 UMETA(DisplayName = "Bleu Tournament"),
    BLEU_ELECTRIQUE UMETA(DisplayName = "Bleu Electrique"),
    VERT_EMPIRE UMETA(DisplayName = "Vert Empire"),
    VERT_JAUNE UMETA(DisplayName = "Vert Jaune"),
    BORDEAUX UMETA(DisplayName = "Bordeaux"),
    
    
    BW_COLORABLE UMETA(DisplayName = "BW Colorable"),
};
const int N_CLOTH_COLORS = BW_COLORABLE + 1;

UCLASS()
class SEFAPOOL_API ATable : public AActor
{
	GENERATED_BODY()
    
    TArray<UMaterial*> clothMaterials;
    
    UPROPERTY()
    UMaterialInstanceDynamic* colorableMaterial;
    
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* secMesh;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* spectatorCameraTarget;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* defaultCueBallPos;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* defaultFoulPos;
    UPROPERTY(VisibleAnywhere)
    USceneComponent* cushionHeight;
    UPROPERTY(VisibleAnywhere)
    USceneComponent* cushionWidth1;
    UPROPERTY(VisibleAnywhere)
    USceneComponent* cushionWidth2;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USplineComponent* TableViewSpline;

	APhysicsSetup* physicsSetup;

	ATable();
	
	FVector GetCueBallDefaultPos();
	FVector GetFoulDefaultPos();
    
    
    void ApplyMaterial(E_TableClothColors color);
    void ApplyColor(FLinearColor color, float adj);
    
    UFUNCTION(BlueprintCallable)
    UMaterialInstanceDynamic* GetMaterialInstance();
    
    UFUNCTION(BlueprintImplementableEvent)
    void BPApplyMaterial(UMaterial* mat);
    
    
    UFUNCTION(BlueprintImplementableEvent)
    void BPApplyDynMaterial(UMaterialInstanceDynamic* mat);

protected:
	virtual void BeginPlay() override;

public:

	virtual ~ATable();

};
