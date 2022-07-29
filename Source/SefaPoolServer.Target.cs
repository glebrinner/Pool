// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class SefaPoolServerTarget : TargetRules
{
    public SefaPoolServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;
		bOverrideBuildEnvironment = true;
		bForceEnableExceptions = true;
		BuildEnvironment = TargetBuildEnvironment.Shared;
		ExtraModuleNames.Add("SefaPool");
	}
}
