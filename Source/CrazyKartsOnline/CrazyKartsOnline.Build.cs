// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CrazyKartsOnline : ModuleRules
{
	public CrazyKartsOnline(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"PhysXVehicles",
			"HeadMountedDisplay",
			"ChaosVehicles",
		});
		
		PublicIncludePaths.AddRange(new string[]
		{
			"CrazyKartsOnline/Public/Karts",
			"CrazyKartsOnline/Public/Components",
		});

		PublicDefinitions.Add("HMD_MODULE_INCLUDED=1");
	}
}
