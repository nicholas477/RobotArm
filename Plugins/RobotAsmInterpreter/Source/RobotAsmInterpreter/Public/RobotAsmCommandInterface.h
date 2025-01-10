// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RobotAsmCommandInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnCommandFinish);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FShouldCommandListStop);

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

	FRobotAsmStateWrapper()
	{
		State = MakeShared<FRobotAsmState>();
	}

	TSharedPtr<FRobotAsmState> State;
};


USTRUCT(BlueprintType, meta = (HasNativeMake = "RobotAsmInterpreterLibrary.MakeOnCommandFinishWrapper"))
struct FOnCommandFinishWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Robot Asm Command|On Command Finish")
	FOnCommandFinish OnCommandFinishDelegate;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "RobotAsmInterpreterLibrary.MakeShouldCommandListStopWrapper"))
struct FShouldCommandListStopWrapper
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Robot Asm Command|Should Command List Stop")
	FShouldCommandListStop ShouldCommandListSTopDelegate;
};

USTRUCT(BlueprintType)
struct FRunCommandOptions
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot Asm Command|Run Command Options")
	TArray<UObject*> CommandList;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot Asm Command|Run Command Options")
	FOnCommandFinish OnFinish;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot Asm Command|Run Command Options")
	FRobotAsmStateWrapper State;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot Asm Command|Run Command Options")
	int32 CommandIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot Asm Command|Run Command Options")
	FShouldCommandListStop ShouldStopDelegate;
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
	void RunCommand(const FRunCommandOptions& Options);

	UFUNCTION(BlueprintNativeEvent, Category = "Robot Asm Command")
	void ConstructCommand(const TArray<FString>& Words);
};
