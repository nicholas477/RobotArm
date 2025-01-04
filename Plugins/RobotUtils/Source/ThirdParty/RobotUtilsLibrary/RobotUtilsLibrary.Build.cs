// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class RobotUtilsLibrary : ModuleRules
{
	public RobotUtilsLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;
		PublicSystemIncludePaths.Add("$(ModuleDir)/orocos_kdl/include");
		PublicSystemIncludePaths.Add("$(ModuleDir)/eigen");

        bEnableExceptions = true;


        if (Target.Platform == UnrealTargetPlatform.Win64)
		{
            // Add the import library
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "orocos_kdl", "lib", "orocos-kdl.lib"));

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("orocos-kdl.dll");

            // Ensure that the DLL is staged along with the executable
            RuntimeDependencies.Add("$(PluginDir)/Binaries/ThirdParty/orocos-kdl.dll");
        }
	}
}
