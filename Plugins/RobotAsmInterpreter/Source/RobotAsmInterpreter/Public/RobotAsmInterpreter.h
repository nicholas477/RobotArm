// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FRobotAsmInterpreterModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

#if WITH_EDITOR
	// Checks the commands in RobotAsmSettings for conflicts.
	// Adds a message to the message log if there are conflicts
	static void CheckCommandConflicts();
#endif
};
