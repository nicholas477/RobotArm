// Copyright 1998-2025 Epic Games, Inc. All Rights Reserved.

#include "SaveGamePluginEditor.h"
#include "GUIExtension.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "Templates/UniquePtr.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/Commands/Commands.h"

#define LOCTEXT_NAMESPACE "FSaveGamePluginEditor"

class FSaveGamePluginEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	FGUIExtension EditorGUIExtension;
};

void FSaveGamePluginEditorModule::StartupModule()
{
	EditorGUIExtension.StartupEditorGUIExtensions();
	//StartupEditorSettingsExtensions();
}

void FSaveGamePluginEditorModule::ShutdownModule()
{
	//ShutdownEditorSettingsExtensions();
	EditorGUIExtension.ShutdownEditorGUIExtensions();
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FSaveGamePluginEditorModule, SaveGamePluginEditor)