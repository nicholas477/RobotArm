// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SaveGamePluginEditor : ModuleRules
{
	public SaveGamePluginEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"SaveGamePlugin",
			"SlateCore",
			"Slate",
            "ToolMenus",
        });

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"CoreUObject",
			"Engine"
		});
	}
}
