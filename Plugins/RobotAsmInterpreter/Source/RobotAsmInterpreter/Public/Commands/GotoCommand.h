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

	virtual void ConstructCommand_Implementation(const TArray<FString>& Words) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Label Command")
	FString LabelName;
};
