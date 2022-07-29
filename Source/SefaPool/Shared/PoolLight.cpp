// Fill out your copyright notice in the Description page of Project Settings.


#include "PoolLight.h"

//! engine does not export the PointLight class, so these are needed
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if WITH_EDITOR
void APointLight::EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown)
{
	const FVector ModifiedScale = DeltaScale * (AActor::bUsePercentageBasedScaling ? 10000.0f : 100.0f);

	FMath::ApplyScaleToFloat(PointLightComponent->AttenuationRadius, ModifiedScale, 1.0f);
	PostEditChange();
}

void APointLight::LoadedFromAnotherClass(const FName& OldClassName)
{
	Super::LoadedFromAnotherClass(OldClassName);

	if (GetLinkerUE4Version() < VER_UE4_REMOVE_LIGHT_MOBILITY_CLASSES)
	{
		static FName PointLightStatic_NAME(TEXT("PointLightStatic"));
		static FName PointLightMovable_NAME(TEXT("PointLightMovable"));
		static FName PointLightStationary_NAME(TEXT("PointLightStationary"));

		check(GetLightComponent() != NULL);

		if (OldClassName == PointLightStatic_NAME)
		{
			GetLightComponent()->Mobility = EComponentMobility::Static;
		}
		else if (OldClassName == PointLightMovable_NAME)
		{
			GetLightComponent()->Mobility = EComponentMobility::Movable;
		}
		else if (OldClassName == PointLightStationary_NAME)
		{
			GetLightComponent()->Mobility = EComponentMobility::Stationary;
		}
	}
}

void APointLight::PostLoad()
{
	Super::PostLoad();

	if (GetLightComponent()->Mobility == EComponentMobility::Static)
	{
		GetLightComponent()->LightFunctionMaterial = NULL;
	}
}

#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
