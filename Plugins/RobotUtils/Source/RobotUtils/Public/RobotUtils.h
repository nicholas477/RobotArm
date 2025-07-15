// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogRobotUtils, Log, All);

DECLARE_STATS_GROUP(TEXT("RobotUtils"), STATGROUP_RobotUtils, STATCAT_Advanced);

class FRobotUtilsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

protected:
	void* OrocosLibraryHandle;
};
