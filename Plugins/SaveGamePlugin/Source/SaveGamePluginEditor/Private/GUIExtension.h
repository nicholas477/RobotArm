// Copyright 1998-2025 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "Framework/Commands/Commands.h"

/**
 *
 */
class FGUIExtension
{
public:
	void StartupEditorGUIExtensions();
	void ShutdownEditorGUIExtensions();

private:
	TSharedRef<FExtender> OnExtendLevelEditorPlayMenu(const TSharedRef<FUICommandList> inCommandList);
	void MapCommands();
	void MakeMenuEntry(FMenuBuilder& MenuBuilder);

	FDelegateHandle PlayMenuExtenderHandle;
	TSharedPtr<FUICommandList> CommandList;
};
