// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotDeviceWorldSubsystem.h"

int32 URobotDeviceWorldSubsystem::AddDevice(FName DeviceName, URobotDeviceComponent* Device)
{
	for (FDeviceArray& DeviceArray : DeviceMap)
	{
		if (DeviceArray.DeviceName == DeviceName)
		{
			return DeviceArray.Devices.Add(Device);
		}
	}
	
	DeviceMap.Add(FDeviceArray({ DeviceName, {Device} }));
	return 0;
}

bool URobotDeviceWorldSubsystem::RemoveDevice(FName DeviceName, int32 DeviceNumber)
{
	for (FDeviceArray& DeviceArray : DeviceMap)
	{
		if (DeviceArray.DeviceName == DeviceName)
		{
			if (DeviceArray.Devices.IsValidIndex(DeviceNumber))
			{
				DeviceArray.Devices[DeviceNumber] = nullptr;
				return true;
			}
		}
	}
	return false;
}

URobotDeviceComponent* URobotDeviceWorldSubsystem::GetDevice(FName DeviceName, int32 DeviceNumber) const
{
	for (const FDeviceArray& DeviceArray : DeviceMap)
	{
		if (DeviceArray.DeviceName == DeviceName)
		{
			if (DeviceArray.Devices.IsValidIndex(DeviceNumber))
			{
				return DeviceArray.Devices[DeviceNumber];
			}
		}
	}
	return nullptr;
}

URobotDeviceComponent* URobotDeviceWorldSubsystem::LookupDevice(const FString& DeviceID) const
{
	static const TArray<TCHAR> Nums = FString("0123456789").GetCharArray();
	
	int32 LastNumberIndex = DeviceID.Len();
	for (int32 i = DeviceID.Len() - 1; i >= 0; --i)
	{
		TCHAR c = DeviceID[i];
		if (Nums.Contains(c))
		{
			LastNumberIndex = i;
		}
		else
		{
			break;
		}
	}
	
	if (LastNumberIndex != DeviceID.Len())
	{
		int32 DeviceNum = FCString::Atoi(*DeviceID.Mid(LastNumberIndex));
		const FString DeviceName = DeviceID.Left(LastNumberIndex); 

		return GetDevice(FName(DeviceName), DeviceNum);
	}

	return nullptr;
}
