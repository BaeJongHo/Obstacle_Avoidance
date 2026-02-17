// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Obstacle_Avoidance : ModuleRules
{
	public Obstacle_Avoidance(ReadOnlyTargetRules Target) : base(Target)
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
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"Obstacle_Avoidance",
			"Obstacle_Avoidance/Actor",
			"Obstacle_Avoidance/GameMode",
			"Obstacle_Avoidance/Variant_Platforming",
			"Obstacle_Avoidance/Variant_Platforming/Animation",
			"Obstacle_Avoidance/Variant_Combat",
			"Obstacle_Avoidance/Variant_Combat/AI",
			"Obstacle_Avoidance/Variant_Combat/Animation",
			"Obstacle_Avoidance/Variant_Combat/Gameplay",
			"Obstacle_Avoidance/Variant_Combat/Interfaces",
			"Obstacle_Avoidance/Variant_Combat/UI",
			"Obstacle_Avoidance/Variant_SideScrolling",
			"Obstacle_Avoidance/Variant_SideScrolling/AI",
			"Obstacle_Avoidance/Variant_SideScrolling/Gameplay",
			"Obstacle_Avoidance/Variant_SideScrolling/Interfaces",
			"Obstacle_Avoidance/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
