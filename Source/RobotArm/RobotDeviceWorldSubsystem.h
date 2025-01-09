// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RobotDeviceWorldSubsystem.generated.h"

class URobotDeviceComponent;

USTRUCT()
struct FDeviceArray
{
	GENERATED_BODY()

	UPROPERTY()
	FName DeviceName;

	UPROPERTY()
	TArray<URobotDeviceComponent*> Devices;
};

/**
 * 
 */
UCLASS()
class ROBOTARM_API URobotDeviceWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<FDeviceArray> DeviceMap;

	// Returns the device number
	int32 AddDevice(FName DeviceName, URobotDeviceComponent* Device);

	bool RemoveDevice(FName DeviceName, int32 DeviceNumber);

	UFUNCTION(BlueprintCallable, Category = "Robot Device World Subsystem")
	void StopAllDevices();

	UFUNCTION(BlueprintPure, Category = "Robot Device World Subsystem")
	URobotDeviceComponent* GetDevice(FName DeviceName, int32 DeviceNumber) const;

	UFUNCTION(BlueprintPure, Category = "Robot Device World Subsystem")
	URobotDeviceComponent* LookupDevice(const FString& DeviceID) const;
};
