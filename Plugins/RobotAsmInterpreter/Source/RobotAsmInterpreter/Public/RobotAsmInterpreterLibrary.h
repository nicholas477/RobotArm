// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RobotAsmCommandInterface.h"
#include "RobotAsmInterpreterLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTASMINTERPRETER_API URobotAsmInterpreterLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter", meta = (WorldContext = "WorldContextObject"))
	static bool InterpretCode(const UObject* WorldContextObject, const FString& Code, FString& OutErrorString, TArray<UObject*>& OutCommands);

	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter", meta=(WorldContext = "WorldContextObject"))
	static void RunCommandList(const UObject* WorldContextObject, const TArray<UObject*>& CommandList, const FOnCommandFinish& OnFinish);

	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter")
	static void RunCommandList_Index(const TArray<UObject*>& CommandList, const FOnCommandFinish& OnFinish, int32 CommandIndex);

	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter")
	static void FinishCommand(const FOnCommandFinish& OnFinish) { OnFinish.ExecuteIfBound(); };
};
