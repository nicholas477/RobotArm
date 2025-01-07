// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotDeviceComponent.h"

#include "RobotDeviceWorldSubsystem.h"

static const FName DefaultDeviceName("Device");

// Sets default values for this component's properties
URobotDeviceComponent::URobotDeviceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;

	DeviceBaseName = DefaultDeviceName;
	DeviceNumber = 0;
	DeviceName = NAME_None;
}

void URobotDeviceComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (GetWorld() == nullptr)
	{
		return;
	}

	if (URobotDeviceWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<URobotDeviceWorldSubsystem>())
	{
		DeviceNumber = Subsystem->AddDevice(DeviceBaseName, this);
		DeviceName = FName(FString::Printf(L"%s%d", *DeviceBaseName.ToString(), DeviceNumber));
	}
}

void URobotDeviceComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (GetWorld() == nullptr)
	{
		return;
	}

	if (URobotDeviceWorldSubsystem* Subsystem = GetWorld()->GetSubsystem<URobotDeviceWorldSubsystem>())
	{
		Subsystem->RemoveDevice(DeviceBaseName, DeviceNumber);
		DeviceNumber = 0;
		DeviceName = NAME_None;
	}
}

int32 URobotDeviceComponent::GetDeviceStatus(int32 MemoryOffset)
{
	if (GetDeviceStatusDelegate.IsBound())
	{
		return GetDeviceStatusDelegate.Execute(this, MemoryOffset);
	}
	return 0;
}
