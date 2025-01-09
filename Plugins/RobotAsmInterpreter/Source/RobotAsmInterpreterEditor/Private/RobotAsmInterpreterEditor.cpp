// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotAsmInterpreterEditor.h"

#include "RobotAsmInterpreterEditorSettings.h"
#include "RobotAsmSettings.h"
#include "GenericRobotAsmCommand.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "GenericPlatform/GenericPlatformProcess.h"


#define LOCTEXT_NAMESPACE "FRobotAsmInterpreterEditorModule"

void FRobotAsmInterpreterEditorModule::StartupModule()
{
	if (UToolMenu* BuildMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Build"))
	{
		FToolMenuSection& Section = BuildMenu->AddSection("RobotAsm", LOCTEXT("RobotAsm", "Robot Assembly"));
		FUIAction PDFAction(
			FExecuteAction::CreateLambda([]()
				{
					GenerateAssemblyManualPDF();
				}),
			FCanExecuteAction::CreateLambda([]()
				{
					return true;
				}),
			FIsActionChecked(),
			FIsActionButtonVisible());

		FToolMenuEntry& Entry = Section.AddMenuEntry(NAME_None, LOCTEXT("GeneratePDFTitle", "Generate Robot Assembly Manual PDF"),
			LOCTEXT("GeneratePDFTitleTooltip", "Generates the PDF manual for the robot assembly language."), FSlateIcon(), PDFAction, EUserInterfaceActionType::Button);
	}
}

void FRobotAsmInterpreterEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FRobotAsmInterpreterEditorModule::GenerateAssemblyManualPDF()
{
	const FString InstructionsPath = URobotAsmInterpreterEditorSettings::GetPDFOutputBasePath() / "instructions";
	const FString ManualPath = URobotAsmInterpreterEditorSettings::GetPDFOutputPath();
	const FString PDFLatexPath = URobotAsmInterpreterEditorSettings::Get()->LatexPath;

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	PlatformFile.CreateDirectory(*URobotAsmInterpreterEditorSettings::GetPDFOutputBasePath());
	PlatformFile.CreateDirectory(*InstructionsPath);

	TSet<FString> CommandFileNames;
	for (TSubclassOf<UObject> Command : URobotAsmSettings::Get()->Commands)
	{
		if (Command == nullptr || Command->GetDefaultObject<UGenericRobotAsmCommand>() == nullptr)
		{
			continue;
		}
		UGenericRobotAsmCommand* AsmCommand = Command->GetDefaultObject<UGenericRobotAsmCommand>();

		FString CommandName = AsmCommand->Command;
		FString CommandShortDescription = AsmCommand->CommandShortDescription.ToString();

		// Header
		FString CommandTexFile = FString::Printf(L"\\addcontentsline{toc}{subsection}{%s}\n"
			"\\Huge\\bfseries %s : \\large %s\\par\n"
			"\\noindent\\rule{ \\textwidth }{2pt}\\par\n"
			"\\vspace{ 0.5cm }\n",
			*CommandName,
			*CommandName,
			*CommandShortDescription);

		// Description
		FString CommandLatexDescription = AsmCommand->CommandLatexDescription.ToString();
		CommandTexFile += FString::Printf(L"\\mdseries\\normalsize %s\n\\par", *CommandLatexDescription);

		// Example (if exists)
		FString CommandExample = AsmCommand->CommandLatexExample.ToString();
		if (!CommandExample.IsEmpty())
		{
			CommandTexFile += FString::Printf(L"\\vspace{ 0.5cm }\n\\Large\\bfseries Example\\par\\vspace{ 0.5cm }\n\\mdseries\\normalsize %s\n", *CommandExample);
		}

		CommandTexFile += "\n\\pagebreak\n";

		IFileManager::Get().Delete(*(InstructionsPath / CommandName + ".tex"));
		FFileHelper::SaveStringToFile(CommandTexFile, *(InstructionsPath / CommandName + ".tex"));

		UE_LOG(LogTemp, Warning, TEXT("Creating tex file for command: %s"), *CommandName);

		CommandFileNames.Add(CommandName + ".tex");
	}

	// File that includes all the asm files
	FString InstructionsFile;
	for (const FString& CommandFile : CommandFileNames)
	{
		InstructionsFile += "\\input{instructions/" + CommandFile + "}\n";
	}
	IFileManager::Get().Delete(*(InstructionsPath / "Instructions.tex"));
	FFileHelper::SaveStringToFile(InstructionsFile, *(InstructionsPath / "Instructions.tex"));

	if (!FPaths::FileExists(*PDFLatexPath))
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find pdflatex at path: %s"), *PDFLatexPath);
		return;
	}

	// Now run pdflatex
	int32 ReturnCode;
	FString StdOut, StdErr;
	UE_LOG(LogTemp, Warning, TEXT("Running latex: %s"), *PDFLatexPath);
	FPlatformProcess::ExecProcess(*PDFLatexPath, *FString::Printf(L"-interaction=batchmode \"%s\"", *ManualPath), &ReturnCode, &StdOut, &StdErr, *URobotAsmInterpreterEditorSettings::GetPDFOutputBasePath());
	
	UE_LOG(LogTemp, Warning, TEXT("PDFLatex return val: %d"), ReturnCode);
	UE_LOG(LogTemp, Warning, TEXT("PDFLatex stdout:"));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *StdOut);
	UE_LOG(LogTemp, Warning, TEXT("PDFLatex stderr:"));
	UE_LOG(LogTemp, Warning, TEXT("%s"), *StdErr);
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRobotAsmInterpreterEditorModule, RobotAsmInterpreterEditor)