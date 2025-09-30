// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/RobotArmGridComponent.h"

#include "Grid/RobotArmGridWorldSubsystem.h"
#include "RobotArmWorldSettings.h"

URobotArmGridComponent::URobotArmGridComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void URobotArmGridComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (auto* Subsystem = URobotArmGridWorldSubsystem::Get(this))
	{
		Subsystem->Grid.Add(GetGridCoordinate(), this);
	}
}

void URobotArmGridComponent::UninitializeComponent()
{
	if (auto* Subsystem = URobotArmGridWorldSubsystem::Get(this))
	{
		Subsystem->Grid.Remove(GetGridCoordinate());
	}

	Super::UninitializeComponent();
}

FRobotArmGridCoordinate URobotArmGridComponent::GetGridCoordinate() const
{
	ARobotArmWorldSettings* WorldSettings = ARobotArmWorldSettings::GetRobotArmWorldSettings(this);
	const FWorldGridSettings GridSettings = WorldSettings ? WorldSettings->GridSettings : FWorldGridSettings();
	return GridSettings.ConvertToGridCoordinate(GetOwner()->GetActorLocation());
}

