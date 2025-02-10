// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RobotAssemblyVM.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ROBOTASMINTERPRETER_API URobotAssemblyVM : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot Assembly VM")
	TArray<UObject*> CommandList;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Robot Assembly VM")
	FString Code;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Robot Assembly VM")
	bool Running;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Robot Assembly VM")
	bool StopRequested;
};
