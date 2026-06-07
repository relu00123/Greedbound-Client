// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Greedbound : ModuleRules
{
    public Greedbound(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "AIModule",
            "StateTreeModule",
            "GameplayStateTreeModule",
            "UMG",
            "NavigationSystem",
            "AIModule",
            "GameplayTasks",
            "HTTP",
            "Json",
            "JsonUtilities",
            "WebSockets",
            "Slate",
            "SlateCore",
            "Niagara",
            "Sockets",
            "NetCore",
            "Networking",
            "MoviePlayer",
	        "AsyncLoadingScreen",
            "DeveloperSettings",
            "CommonInput",
            "PropertyPath",
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",
        });

        PublicIncludePaths.AddRange(new string[] {
            "Greedbound",
            "Greedbound/Variant_Platforming",
            "Greedbound/Variant_Combat",
            "Greedbound/Variant_Combat/AI",
            "Greedbound/Variant_SideScrolling",
            "Greedbound/Variant_SideScrolling/Gameplay",
            "Greedbound/Variant_SideScrolling/AI",
            "Greedbound/GB"
        });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}
