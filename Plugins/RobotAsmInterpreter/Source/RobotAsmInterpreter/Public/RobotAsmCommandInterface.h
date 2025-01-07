// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RobotAsmCommandInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnCommandFinish);

USTRUCT(BlueprintType)
struct FRobotAsmState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot Assembly Interpreter State")
	TMap<FName, int32> Variables;
};

USTRUCT(BlueprintType)
struct FRobotAsmStateWrapper
{
	GENERATED_BODY()

	TSharedPtr<FRobotAsmState> State;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URobotAsmCommandInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ROBOTASMINTERPRETER_API IRobotAsmCommandInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Robot Asm Command")
	void SetCommandWorld(UWorld* World);

	UFUNCTION(BlueprintNativeEvent, Category = "Robot Asm Command")
	void RunCommand(const TArray<UObject*>& CommandList, const FOnCommandFinish& OnFinish, FRobotAsmStateWrapper State);

	UFUNCTION(BlueprintNativeEvent, Category = "Robot Asm Command")
	void ConstructCommand(const TArray<FString>& Words);
};
