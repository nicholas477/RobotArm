// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class RobotArm : ModuleRules
{
	public RobotArm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"RobotUtils",
			"RobotAsmInterpreter",
			"Mover",
			"RHI",
			"RenderCore"
        });
	}
}
