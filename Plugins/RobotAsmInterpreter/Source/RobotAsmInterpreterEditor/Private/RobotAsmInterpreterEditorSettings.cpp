// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotAsmInterpreterEditorSettings.h"

URobotAsmInterpreterEditorSettings::URobotAsmInterpreterEditorSettings()
{
	PDFOutputPath = "PDF/Manual.tex";
	LatexPath = "C:/Users/Nicholas Chalkley/AppData/Local/Programs/MiKTeX/miktex/bin/x64/pdflatex.exe";
}

FString URobotAsmInterpreterEditorSettings::GetPDFOutputPath()
{
	const URobotAsmInterpreterEditorSettings* Settings = Get();

	return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / Settings->PDFOutputPath);
}

FString URobotAsmInterpreterEditorSettings::GetPDFOutputBasePath()
{
	return FPaths::GetPath(GetPDFOutputPath());
}
