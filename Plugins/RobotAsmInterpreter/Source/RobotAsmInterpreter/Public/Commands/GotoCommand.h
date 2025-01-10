// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericRobotAsmCommand.h"
#include "GotoCommand.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTASMINTERPRETER_API UGotoCommand : public UGenericRobotAsmCommand
{
	GENERATED_BODY()
	
public:
	UGotoCommand();

	virtual bool ConstructCommand_Implementation(const TArray<FString>& Words) override;

	UFUNCTION(BlueprintNativeEvent, Category = "Goto Command")
	bool ShouldBranch(const TArray<UObject*>& CommandList, FRobotAsmStateWrapper State);

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Label Command")
	FString LabelName;
};
