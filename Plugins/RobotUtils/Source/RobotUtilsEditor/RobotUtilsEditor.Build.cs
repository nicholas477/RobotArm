// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RobotUtilsEditor : ModuleRules
{
	public RobotUtilsEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"RobotUtilsLibrary",
				"RobotUtils",
				"Projects",
                "PropertyEditor",
				"Slate",
				"SlateCore"
            }
		);
	}
}
