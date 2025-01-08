// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RobotAsmSettings.generated.h"

/**
 * 
 */
UCLASS(config = Engine, defaultconfig)
class ROBOTASMINTERPRETER_API URobotAsmSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(config, EditAnywhere, Category = "Robot Assembly Settings", meta = (MustImplement = "RobotAsmCommandInterface"))
	TSet<TSubclassOf<UObject>> Commands;

	static TSubclassOf<UObject> GetCommand(const FString& CommandName);

	static URobotAsmSettings* Get() { return GetMutableDefault<URobotAsmSettings>(); }

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
