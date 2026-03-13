// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Thesis : ModuleRules
{
	public Thesis(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        MinFilesUsingPrecompiledHeaderOverride = 1;
        bUseUnity = false;

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
			"Slate",
            "AnimGraphRuntime"
        });

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Thesis",
			"Thesis/Variant_Platforming",
			"Thesis/Variant_Platforming/Animation",
			"Thesis/Variant_Combat",
			"Thesis/Variant_Combat/AI",
			"Thesis/Variant_Combat/Animation",
			"Thesis/Variant_Combat/Gameplay",
			"Thesis/Variant_Combat/Interfaces",
			"Thesis/Variant_Combat/UI",
			"Thesis/Variant_SideScrolling",
			"Thesis/Variant_SideScrolling/AI",
			"Thesis/Variant_SideScrolling/Gameplay",
			"Thesis/Variant_SideScrolling/Interfaces",
			"Thesis/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
