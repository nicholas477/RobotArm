// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grid/RobotArmGridWorldSubsystem.h"
#include "RobotArmGridComponent.generated.h"


UCLASS( ClassGroup=(Grid), meta=(BlueprintSpawnableComponent) )
class ROBOTARM_API URobotArmGridComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URobotArmGridComponent(const FObjectInitializer& ObjectInitializer);

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	UFUNCTION(BlueprintPure)
	FRobotArmGridCoordinate GetGridCoordinate() const;
};
