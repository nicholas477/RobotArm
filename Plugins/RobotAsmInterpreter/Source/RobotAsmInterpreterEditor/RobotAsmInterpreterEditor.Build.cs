// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RobotAsmInterpreterEditor : ModuleRules
{
	public RobotAsmInterpreterEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"DeveloperSettings",
				"RobotAsmInterpreter",
				"DeveloperSettings"
			}
		);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"ToolMenus"
            }
		);

        RuntimeDependencies.Add("$(ProjectDir)/PDF/Manual.pdf");
    }
}
