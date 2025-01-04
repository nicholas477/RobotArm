// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RobotUtils : ModuleRules
{
	public RobotUtils(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableExceptions = true;

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RobotUtilsLibrary",
				"Projects",
				"RHI",
				"RenderCore"
			}
		);
	}
}
