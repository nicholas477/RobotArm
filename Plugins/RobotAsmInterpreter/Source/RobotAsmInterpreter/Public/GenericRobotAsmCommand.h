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
class ROBOTASMINTERPRETER_API UGenericRobotAsmCommand : public UObject, public FTickableGameObject, public IRobotAsmCommandInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Generic Robot Assembly Command")
	FString Command;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Generic Robot Assembly Command", meta=(DisplayName="Tick"))
	void K2_Tick(float DeltaTime);

	void SetCommandWorld_Implementation(UWorld* InWorld) override { World = InWorld; }

	virtual UWorld* GetWorld() const override { return World; };
	virtual UWorld* GetTickableGameObjectWorld() const override { return World; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UGenericRobotAsmCommand, STATGROUP_Tickables); }

	UPROPERTY()
	UWorld* World;

	// Track The last frame number we were ticked.
	uint64 LastTickFrame = INDEX_NONE;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Generic Robot Assembly Command")
	bool TickCommand = false;

	virtual bool IsTickable() const override { return TickCommand; }
};
