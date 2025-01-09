// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RobotAsmInterpreterEditorSettings.generated.h"

/**
 * 
 */
UCLASS(config=Editor, defaultconfig)
class ROBOTASMINTERPRETEREDITOR_API URobotAsmInterpreterEditorSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	URobotAsmInterpreterEditorSettings();

	// PDF output path relative to the project folder
	UPROPERTY(config, EditAnywhere, Category = "Robot Assembly Editor Settings")
	FString PDFOutputPath;

	// Path of pdflatex
	UPROPERTY(config, EditAnywhere, Category = "Robot Assembly Editor Settings")
	FString LatexPath;

	static URobotAsmInterpreterEditorSettings* Get() { return GetMutableDefault<URobotAsmInterpreterEditorSettings>(); }

	// Gets the full output path of the PDF
	static FString GetPDFOutputPath();
	static FString GetPDFOutputBasePath();
};
