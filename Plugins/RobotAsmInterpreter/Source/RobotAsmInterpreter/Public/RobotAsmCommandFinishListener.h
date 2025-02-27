// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RobotAsmCommandInterface.h"
#include "RobotAsmInterpreterLibrary.h"
#include "RobotAsmCommandFinishListener.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTASMINTERPRETER_API URobotAsmCommandFinishListener : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FRunCommandOptions Options;

	UFUNCTION()
	void OnCommandFinish();
};
