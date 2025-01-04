// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotUtils.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#include "RobotUtilsKDL.h"

DEFINE_LOG_CATEGORY(LogRobotUtils);

#define LOCTEXT_NAMESPACE "FRobotUtilsModule"

void FRobotUtilsModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("RobotUtils")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/orocos-kdl.dll"));
#endif // PLATFORM_WINDOWS

	OrocosLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	check(OrocosLibraryHandle != nullptr);
}

void FRobotUtilsModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(OrocosLibraryHandle);
	OrocosLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRobotUtilsModule, RobotUtils)
