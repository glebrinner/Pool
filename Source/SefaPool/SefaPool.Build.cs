// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class SefaPool : ModuleRules
{
	public SefaPool(ReadOnlyTargetRules Target) : base(Target)
	{
		//PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	    
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Networking", "Sockets"});
        //if (Target.Platform != UnrealTargetPlatform.IOS && Target.Platform != UnrealTargetPlatform.Android)
        //{
        PublicDependencyModuleNames.Add("Agones");

        //}
        
        if (Target.Platform == UnrealTargetPlatform.Linux ||
            Target.Platform == UnrealTargetPlatform.Win64)
        {
                PublicDependencyModuleNames.Add("RedisPlugin");
        }

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
        PrivateDependencyModuleNames.AddRange(new string[] { "Http", "Json", "JsonUtilities" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
        
        PrivateDependencyModuleNames.AddRange(new string[] { "OnlineSubsystem", "OnlineSubsystemUtils"});
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
        if (Target.Platform == UnrealTargetPlatform.IOS)
        {
            DynamicallyLoadedModuleNames.AddRange(new string[] { "OnlineSubsystemIOS" });
            PublicDependencyModuleNames.Add("Launch");
        }
        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            //PrivateDependencyModuleNames.Add("OnlineSubsystem");
            //DynamicallyLoadedModuleNames.Add("OnlineSubsystemGooglePlay");
            PublicDependencyModuleNames.Add("Launch");
        }
	}
}
