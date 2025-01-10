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
	static void RunCommandList(const UObject* WorldContextObject, const FRunCommandOptions& Options);

	static void RunCommandList_Index(const FRunCommandOptions& Options);

	UFUNCTION(BlueprintPure, Category = "Robot Assembly Interpreter")
	static bool FindLabelIndex(const TArray<UObject*>& CommandList, const FString& Label, int32& OutIndex);

	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter")
	static void FinishCommand(const FOnCommandFinish& OnFinish) { OnFinish.ExecuteIfBound(); };

	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter")
	static bool ShouldCommandListStop(const FShouldCommandListStop& ShouldStopDelegate);

	UFUNCTION(BlueprintPure, Category = "Robot Assembly Interpreter")
	static FOnCommandFinishWrapper MakeOnCommandFinishWrapper(const FOnCommandFinish& OnFinish) { return FOnCommandFinishWrapper{OnFinish}; }

	UFUNCTION(BlueprintPure, Category = "Robot Assembly Interpreter")
	static FShouldCommandListStopWrapper MakeShouldCommandListStopWrapper(const FShouldCommandListStop& ShouldCommandListStop) { return FShouldCommandListStopWrapper{ ShouldCommandListStop }; }

	UFUNCTION(BlueprintPure, Category = "Robot Assembly Interpreter")
	static FRobotAsmState ReadAssemblyState(const FRobotAsmStateWrapper& Wrapper) { return *Wrapper.State; };

	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter")
	static void WriteAssemblyState(FRobotAsmStateWrapper Wrapper, const FRobotAsmState& State) { *Wrapper.State = State; };

	UFUNCTION(BlueprintCallable, Category = "Robot Assembly Interpreter")
	static void WriteAssemblyStateVariable(FRobotAsmStateWrapper Wrapper, FName VariableName, int32 Value);

	UFUNCTION(BlueprintPure, Category = "Robot Assembly Interpreter")
	static int32 ReadAssemblyStateVariable(FRobotAsmStateWrapper Wrapper, FName VariableName);
};
