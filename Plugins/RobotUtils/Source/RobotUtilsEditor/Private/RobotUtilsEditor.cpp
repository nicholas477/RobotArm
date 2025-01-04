// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotUtilsEditor.h"

#include "RobotUtilsEditorPropertyTypes.h"

DEFINE_LOG_CATEGORY(LogRobotUtilsEditor);

#define LOCTEXT_NAMESPACE "FRobotUtilsEditorModule"

void FRobotUtilsEditorModule::StartupModule()
{
	//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.RegisterCustomPropertyTypeLayout("RobotJoint", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRobotJointDetails::MakeInstance));
	//PropertyModule.RegisterCustomPropertyTypeLayout("RobotSegment", FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRobotSegmentDetails::MakeInstance));
}

void FRobotUtilsEditorModule::ShutdownModule()
{
	//FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//PropertyModule.UnregisterCustomPropertyTypeLayout("RobotJoint");
	//PropertyModule.UnregisterCustomPropertyTypeLayout("RobotSegment");
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRobotUtilsEditorModule, RobotUtilsEditor)
