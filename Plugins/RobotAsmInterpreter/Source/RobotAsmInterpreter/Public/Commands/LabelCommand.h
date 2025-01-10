// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericRobotAsmCommand.h"
#include "LabelCommand.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTASMINTERPRETER_API ULabelCommand : public UGenericRobotAsmCommand
{
	GENERATED_BODY()
	
public:
	ULabelCommand();

	virtual void RunCommand_Implementation(const FRunCommandOptions& Options) override;
	virtual void ConstructCommand_Implementation(const TArray<FString>& Words) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Label Command")
	FString LabelName;
};
