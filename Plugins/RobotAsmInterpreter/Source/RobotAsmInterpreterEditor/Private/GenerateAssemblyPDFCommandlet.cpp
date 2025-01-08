// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateAssemblyPDFCommandlet.h"

#include "RobotAsmSettings.h"
#include "GenericRobotAsmCommand.h"
#include "Misc/FileHelper.h"

int32 UGenerateAssemblyPDFCommandlet::Main(const FString& Params)
{
	TArray<FString> Tokens;
	TArray<FString> Switches;
	TMap<FString, FString> ParamsMap;
	UCommandlet::ParseCommandLine(*Params, Tokens, Switches, ParamsMap);

	UE_LOG(LogTemp, Warning, TEXT("Hello!"));

	if (!ParamsMap.Contains("output"))
	{
		UE_LOG(LogTemp, Display, TEXT("    -Output=<path/to/folder>              Output folder for latex files (required)"));
		return 1;
	}
	FString OutputPath = ParamsMap["output"];
	if (!FPaths::DirectoryExists(OutputPath))
	{
		UE_LOG(LogTemp, Display, TEXT("Output directory does not exist"));
		return 1;
	}
	FPaths::NormalizeDirectoryName(OutputPath);

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

		IFileManager::Get().Delete(*(OutputPath / CommandName + ".tex"));
		FFileHelper::SaveStringToFile(CommandTexFile, *(OutputPath / CommandName + ".tex"));

		UE_LOG(LogTemp, Warning, TEXT("Creating tex file for command: %s"), *CommandName);

		CommandFileNames.Add(CommandName + ".tex");
	}

	// File that includes all the asm files
	FString InstructionsFile;
	for (const FString& CommandFile : CommandFileNames)
	{
		InstructionsFile += "\\input{instructions/" + CommandFile + "}\n";
	}
	IFileManager::Get().Delete(*(OutputPath / "Instructions.tex"));
	FFileHelper::SaveStringToFile(InstructionsFile, *(OutputPath / "Instructions.tex"));

	FPlatformProcess::Sleep(10.0);
	return 0;
}
