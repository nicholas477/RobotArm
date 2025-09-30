// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/RobotArmGridWorldSubsystem.h"

#include "RobotArmWorldSettings.h"

FRobotArmGridCoordinate URobotArmGridWorldSubsystem::GetGridCoordinate(const UObject* WorldContextObject, const FVector& WorldLocation)
{
	ARobotArmWorldSettings* WorldSettings = ARobotArmWorldSettings::GetRobotArmWorldSettings(WorldContextObject);
	const FWorldGridSettings GridSettings = WorldSettings ? WorldSettings->GridSettings : FWorldGridSettings();
	return GridSettings.ConvertToGridCoordinate(WorldLocation);
}

URobotArmGridWorldSubsystem* URobotArmGridWorldSubsystem::Get(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return World->GetSubsystem<ThisClass>();
	}

	return nullptr;
}