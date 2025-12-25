// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class GAS_System_Study : ModuleRules
{
	public GAS_System_Study(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] 
		{ 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"GameplayAbilities",
			"GameplayTasks",
			"GameplayTags",
			"UMG",
			"Slate",
			"SlateCore",
			"Niagara",
			"LevelSequence"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

	}
}
