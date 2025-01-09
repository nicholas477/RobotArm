// Fill out your copyright notice in the Description page of Project Settings.


#include "GenerateAssemblyPDFCommandlet.h"

#include "RobotAsmInterpreterEditor.h"

int32 UGenerateAssemblyPDFCommandlet::Main(const FString& Params)
{
	FRobotAsmInterpreterEditorModule::GenerateAssemblyManualPDF();

	FPlatformProcess::Sleep(30.0);
	return 0;
}
