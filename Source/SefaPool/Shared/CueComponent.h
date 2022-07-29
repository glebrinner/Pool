// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "CueComponent.generated.h"

/**
 * 
 */
const int CHALK_RETENTION_SHOTS = 5;
const size_t COMPONENT_0_N = 5;
const uint8 COMPONENT_0_VERSIONS[COMPONENT_0_N] = {1, 2, 3, 8, 9};
const uint8 PREDEFINED_CUES_N = 20;
const uint8 PREDEFINED_CHALKS_N = 4;
const uint8 PREDEFINED_CLOTHS_N = 28;
const uint8 PREDEFINED_TOKENPAKS_N = 8;
const uint8 PREDEFINED_BUNDLES_N = 7;
const uint8 COMPONENT_0_PER_CUE[PREDEFINED_CUES_N + 1] = {200, 0, 1, 2, 0, 0, 0, 0, 3, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//                                                  ^------^-- so we do not have to add 1 to the cue number

USTRUCT()
struct FCueInfo
{
    GENERATED_USTRUCT_BODY()
    
    UPROPERTY()
    int cueNum = 1;
    
    UPROPERTY()
    int chalkNum = 1;
    
    UPROPERTY()
    int shotsSinceChalked = CHALK_RETENTION_SHOTS + 1;
public:
    FCueInfo();
    
    void  ChangeCueNum(int num);
    void  ChangeChalkNum(int num);
    int   GetCueNum();
    int   GetChalkNum();
    float GetPower();
    float GetEffect();
    float GetTime();
    float static GetPower(int cueNum);
    float static GetEffect(int num);
    float static GetTime(int num);
    void ChalkUp();
    void Unchalk();
    bool IsChalked();
    int ShotsBeforeRechalking();
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SEFAPOOL_API UCueComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
    
    TArray<UMaterial*> component0_materials;
    TArray<UMaterial*> component1_materials;
    UMaterial*         component2_material;
    UMaterial*         component3_material;
    UMaterial*         component4_material;
    UMaterial*         component5_material;
    UMaterial*         component6_material;
    
public:
    UCueComponent();
    
    UFUNCTION(BlueprintCallable)
    void SelectCue(int n);
    
    UFUNCTION(BlueprintCallable)
    void SelectComp0(int n);
    
    UFUNCTION(BlueprintCallable)
    void SelectComp1(int n);
};

