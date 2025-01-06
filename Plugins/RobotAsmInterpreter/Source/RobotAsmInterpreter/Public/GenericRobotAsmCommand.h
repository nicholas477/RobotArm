// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RobotAsmCommandInterface.h"
#include "GenericRobotAsmCommand.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ROBOTASMINTERPRETER_API UGenericRobotAsmCommand : public UObject, public IRobotAsmCommandInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generic Robot Assembly Command")
	FString Command;

	void SetCommandWorld_Implementation(UWorld* InWorld) override { World = InWorld; }

	virtual UWorld* GetWorld() const override { return World; };

	UPROPERTY()
	UWorld* World;
};
