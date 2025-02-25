// Copyright 1998-2025 Epic Games, Inc. All Rights Reserved.


#include "GUIExtension.h"

#include "SaveGameSettings.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Styling/AppStyle.h"
#include "SaveGameSystem.h"
#include "PlatformFeatures.h"
#include "SaveFile/SaveFileManager.h"

#define LOCTEXT_NAMESPACE "SavePluginEditorGUIExtension"

FDelegateHandle LevelViewportExtenderHandle;

class MenuToolCommands : public TCommands<MenuToolCommands>
{
public:
	MenuToolCommands()
		: TCommands<MenuToolCommands>(
			TEXT("MenuTool"), // Context name for fast lookup
			FText::FromString("Example Menu tool"), // Context name for displaying
			NAME_None,   // No parent context
			FAppStyle::GetAppStyleSetName() // Icon Style Set
		)
	{
	}

	virtual void RegisterCommands() override
	{
		UI_COMMAND(SaveSystem_EnableSaving, "Enable Game Saving", "When enabled, the game will write to the save game file.", EUserInterfaceActionType::ToggleButton, FInputChord());
		UI_COMMAND(SaveSystem_EnableLoading, "Enable Game Loading", "When enabled, the game will read from the save game file.", EUserInterfaceActionType::ToggleButton, FInputChord());
		UI_COMMAND(SaveSystem_DeleteMapSave, "Delete Map Save", "Deletes the save data for this map", EUserInterfaceActionType::Button, FInputChord());
		UI_COMMAND(SaveSystem_DeleteSave, "Delete Game Save", "Deletes the current game save", EUserInterfaceActionType::Button, FInputChord());
	}

public:
	TSharedPtr<FUICommandInfo> SaveSystem_EnableSaving;
	TSharedPtr<FUICommandInfo> SaveSystem_EnableLoading;
	TSharedPtr<FUICommandInfo> SaveSystem_DeleteMapSave;
	TSharedPtr<FUICommandInfo> SaveSystem_DeleteSave;
};

void FGUIExtension::StartupEditorGUIExtensions()
{
	CommandList = MakeShareable(new FUICommandList);
	MenuToolCommands::Register();
	MapCommands();

	UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
	FToolMenuSection& Section = Menu->FindOrAddSection("SavePluginEditor");

	Section.AddSeparator("SavePluginEditorSpacer");

	FToolMenuEntry Entry = FToolMenuEntry::InitComboButton(
		"SavePluginEditorSettingsMenu",
		FUIAction(),
		FOnGetContent::CreateLambda([this] { 
			FMenuBuilder MenuBuilder(true, CommandList);
			MakeMenuEntry(MenuBuilder);
			return MenuBuilder.MakeWidget();
			}),
		INVTEXT("Save Plugin Settings"),
		INVTEXT("Configure save plugin settings"),
		FSlateIcon(FAppStyle::Get().GetStyleSetName(), TEXT("Icons.Save")),
		false);
	Entry.StyleNameOverride = "CalloutToolbar";
	Entry.SetCommandList(CommandList);

	Section.AddEntry(Entry);
}

void FGUIExtension::ShutdownEditorGUIExtensions()
{
	// Unload editor extension
	if (UToolMenus::Get())
	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.User");
		if (Menu)
		{
			Menu->RemoveSection("SavePluginEditor");
		}
	}
}

TSharedRef<FExtender> FGUIExtension::OnExtendLevelEditorPlayMenu(const TSharedRef<FUICommandList> inCommandList)
{
	TSharedRef<FExtender> Extender(new FExtender());

	Extender->AddMenuExtension(
		"LevelEditorPlaySettings",
		EExtensionHook::After,
		CommandList,
		FMenuExtensionDelegate::CreateRaw(this, &FGUIExtension::MakeMenuEntry)
	);

	return Extender;
}

static bool SaveSystem_EnableSaving_IsChecked()
{
	return GetMutableDefault<USaveGameSettings>()->bEnableSaving;
}

static void SaveSystem_EnableSaving_Toggled()
{
	USaveGameSettings* MutableDefault = GetMutableDefault<USaveGameSettings>();
	MutableDefault->bEnableSaving = !MutableDefault->bEnableSaving;
	MutableDefault->SaveConfig(16384U, *MutableDefault->GetDefaultConfigFilename());
}

static bool SaveSystem_EnableLoading_IsChecked()
{
	return GetMutableDefault<USaveGameSettings>()->bEnableLoading;
}

static void SaveSystem_EnableLoading_Toggled()
{
	USaveGameSettings* MutableDefault = GetMutableDefault<USaveGameSettings>();
	MutableDefault->bEnableLoading = !MutableDefault->bEnableLoading;
	MutableDefault->SaveConfig(16384U, *MutableDefault->GetDefaultConfigFilename());
}

static void SaveSystem_DeleteSave()
{
	const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::YesNo, INVTEXT("Are you sure you want to delete your game save?"));
	if (Choice == EAppReturnType::Yes)
	{
		//GetMutableDefault<USaveSystem>()->DeleteSave();
		ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
		if (SaveSystem)
		{
			SaveSystem->DeleteGame(false, TEXT("SaveGame"), 0);
			SaveSystem->DeleteGame(false, TEXT("SaveGame.json"), 0);
		}
	}
}

static bool SaveSystem_DeleteSave_IsEnabled()
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (SaveSystem)
	{
		return SaveSystem->DoesSaveGameExist(TEXT("SaveGame"), 0);
	}
	return false;
}

static void SaveSystem_DeleteMapSave()
{
	if (GEditor == nullptr)
	{
		return;
	}

	UWorld* World = GEditor->GetEditorWorldContext().World();
	if (World == nullptr)
	{
		return;
	}

	const EAppReturnType::Type Choice = FMessageDialog::Open(EAppMsgType::YesNo, INVTEXT("Are you sure you want to delete your map save?"));
	if (Choice == EAppReturnType::Yes)
	{
		FSaveFileManager BinaryFileManager = FSaveFileManager("SaveGame");
		FJsonSaveFileManager JsonFileManager = FJsonSaveFileManager("SaveGame.json");

		FString WorldName = "Levels" / World->GetOutermost()->GetLoadedPath().GetPackageName();

		BinaryFileManager.LoadGameFile();
		JsonFileManager.LoadGameFile();

		BinaryFileManager.DeleteFile(FName(WorldName + ".sav"));
		JsonFileManager.DeleteFile(FName(WorldName + ".json"));

		BinaryFileManager.SaveGameFile();
		JsonFileManager.SaveGameFile();
	}
}

static bool SaveSystem_DeleteMapSave_IsEnabled()
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (SaveSystem)
	{
		return SaveSystem->DoesSaveGameExist(TEXT("SaveGame"), 0);
	}
	return false;
}

void FGUIExtension::MapCommands()
{
	const auto& Commands = MenuToolCommands::Get();

	CommandList->MapAction(
		Commands.SaveSystem_EnableSaving,
		FExecuteAction::CreateStatic(&SaveSystem_EnableSaving_Toggled),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&SaveSystem_EnableSaving_IsChecked));

	CommandList->MapAction(
		Commands.SaveSystem_EnableLoading,
		FExecuteAction::CreateStatic(&SaveSystem_EnableLoading_Toggled),
		FCanExecuteAction(),
		FIsActionChecked::CreateStatic(&SaveSystem_EnableLoading_IsChecked));

	CommandList->MapAction(
		Commands.SaveSystem_DeleteMapSave,
		FExecuteAction::CreateStatic(&SaveSystem_DeleteMapSave),
		FCanExecuteAction::CreateStatic(&SaveSystem_DeleteMapSave_IsEnabled));

	CommandList->MapAction(
		Commands.SaveSystem_DeleteSave,
		FExecuteAction::CreateStatic(&SaveSystem_DeleteSave),
		FCanExecuteAction::CreateStatic(&SaveSystem_DeleteSave_IsEnabled));
}

void FGUIExtension::MakeMenuEntry(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection(NAME_None, LOCTEXT("SaveSystemSubMenuLabel", "Save Game Plugin"));
	MenuBuilder.AddMenuEntry(MenuToolCommands::Get().SaveSystem_EnableSaving);
	MenuBuilder.AddMenuEntry(MenuToolCommands::Get().SaveSystem_EnableLoading);
	MenuBuilder.AddMenuSeparator();
	MenuBuilder.AddMenuEntry(MenuToolCommands::Get().SaveSystem_DeleteMapSave);
	MenuBuilder.AddMenuEntry(MenuToolCommands::Get().SaveSystem_DeleteSave);
	MenuBuilder.EndSection();
}

#undef LOCTEXT_NAMESPACE