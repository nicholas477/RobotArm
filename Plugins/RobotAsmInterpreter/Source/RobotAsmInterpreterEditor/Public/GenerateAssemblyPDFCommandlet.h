// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "GenerateAssemblyPDFCommandlet.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTASMINTERPRETEREDITOR_API UGenerateAssemblyPDFCommandlet : public UCommandlet
{
	GENERATED_BODY()
	
public:
	virtual int32 Main(const FString& Params) override;
};
