// Fill out your copyright notice in the Description page of Project Settings.


#include "CueComponent.h"
#include "Misc/CString.h"
#include "stdio.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Engine/StaticMesh.h"
#include "SharedDefs.h"


UCueComponent::UCueComponent() :
    component0_materials (),
    component1_materials (),
    component2_material  (nullptr),
    component3_material  (nullptr),
    component4_material  (nullptr),
    component5_material  (nullptr),
    component6_material  (nullptr)
{
    TCHAR materialPath[1000] = {};
    for (int i = 0; i < COMPONENT_0_N; i++)
    {
        FCString::Sprintf(materialPath, TEXT("/Game/Cue/Cue_0_v%d.Cue_0_v%d"), COMPONENT_0_VERSIONS[i], COMPONENT_0_VERSIONS[i]);
        ConstructorHelpers::FObjectFinder<UMaterial> CueMaterial (materialPath);
        component0_materials.Add(CueMaterial.Object);
    }
    component1_materials.Add(nullptr); // so we do not have to add 1 to the cue number
    for (int i = 0; i < PREDEFINED_CUES_N; i++)
    {
        FCString::Sprintf(materialPath, TEXT("/Game/Cue/Cue_1_v%d.Cue_1_v%d"), i + 1, i + 1);
        ConstructorHelpers::FObjectFinder<UMaterial> CueMaterial(materialPath);
        component1_materials.Add(CueMaterial.Object);
    }
#define LOAD_CUE_MATERIAL(n)\
ConstructorHelpers::FObjectFinder<UMaterial> CueMaterial_##n(TEXT("/Game/Cue/Cue_" #n "_v1.Cue_" #n "_v1")); \
component##n##_material = CueMaterial_##n.Object;
    
    LOAD_CUE_MATERIAL(2)
    LOAD_CUE_MATERIAL(3)
    LOAD_CUE_MATERIAL(4)
    LOAD_CUE_MATERIAL(5)
    LOAD_CUE_MATERIAL(6)
#undef LOAD_CUE_MATERIAL
    
    static ConstructorHelpers::FObjectFinder<UStaticMesh> CueMeshAsset(TEXT("StaticMesh'/Game/Cue/CueFix.CueFix'"));
    SetStaticMesh(CueMeshAsset.Object);
    SetWorldScale3D(FVector(0.8f));
    
}

void UCueComponent::SelectCue(int n)
{
    if (n < 1 || n > 20) return;
    SetMaterial(0, component0_materials[COMPONENT_0_PER_CUE[n]]);
    SetMaterial(1, component1_materials[n]);
    SetMaterial(2, component2_material);
    SetMaterial(3, component3_material);
    SetMaterial(4, component4_material);
    SetMaterial(5, component5_material);
    SetMaterial(6, component6_material);
}
void UCueComponent::SelectComp0(int n)
{
    if (n < 0 || n > COMPONENT_0_N - 1) return;
    SetMaterial(0, component0_materials[n]);
}
void UCueComponent::SelectComp1(int n)
{
    if (n < 1 || n > PREDEFINED_CUES_N) return;
    SetMaterial(1, component1_materials[n]);
}


FCueInfo::FCueInfo() :
    cueNum            (1),
    shotsSinceChalked (CHALK_RETENTION_SHOTS + 1)
{}

void FCueInfo::ChangeCueNum(int num)
{
    if (num < 1 || num > PREDEFINED_CUES_N) return;
    cueNum = num;
}

void FCueInfo::ChangeChalkNum(int num)
{
    if (num < 1 || num > PREDEFINED_CHALKS_N) return;
    chalkNum = num;
}

int FCueInfo::GetCueNum()
{
    return cueNum;
}

int FCueInfo::GetChalkNum()
{
    return chalkNum;
}

float FCueInfo::GetPower()
{
    return 0.3f + (-0.00356302f*cueNum*cueNum*cueNum +
                   0.185202f*cueNum*cueNum +
                   1.08748f*cueNum + 1.71146f) / 100.0f * 0.7f;
}

float FCueInfo::GetPower(int cueNum)
{
    return 0.3f + (-0.00356302f * cueNum * cueNum * cueNum +
        0.185202f * cueNum * cueNum +
        1.08748f * cueNum + 1.71146f) / 100.0f * 0.7f;
}

float FCueInfo::GetEffect()
{
    return 0.1f + (-0.00356302f*cueNum*cueNum*cueNum +
                          0.185202f*cueNum*cueNum +
                          1.08748f*cueNum + 1.71146f) / 100.0f * 0.9f;
}

float FCueInfo::GetEffect(int cueNum)
{
    return 0.1f + (-0.00356302f*cueNum*cueNum*cueNum +
                          0.185202f*cueNum*cueNum +
                          1.08748f*cueNum + 1.71146f) / 100.0f * 0.9f;
}

float FCueInfo::GetTime()
{
    if (cueNum == 1) return MIN_SHOT_TIME;
    else return MIN_SHOT_TIME + (MAX_SHOT_TIME - MIN_SHOT_TIME) * (2.0f * cueNum + 13.0f) / 100.0f * 2.0f;
}

float FCueInfo::GetTime(int cueNum)
{
    if (cueNum == 1) return MIN_SHOT_TIME;
    else return MIN_SHOT_TIME + (MAX_SHOT_TIME - MIN_SHOT_TIME) * (2.0f * cueNum + 13.0f) / 100.0f * 2.0f;
}

void FCueInfo::ChalkUp()
{
    shotsSinceChalked = 0;
}

void FCueInfo::Unchalk()
{
    shotsSinceChalked++;
}

bool FCueInfo::IsChalked()
{
    return shotsSinceChalked < CHALK_RETENTION_SHOTS;
}

int FCueInfo::ShotsBeforeRechalking()
{
    return CHALK_RETENTION_SHOTS - shotsSinceChalked > 0 ? CHALK_RETENTION_SHOTS - shotsSinceChalked : 0;
}
