// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Grid/GridTypes.h"
#include "RobotArmGridWorldSubsystem.generated.h"

class URobotArmGridComponent;

/**
 * 
 */
UCLASS()
class ROBOTARM_API URobotArmGridWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TMap<FRobotArmGridCoordinate, URobotArmGridComponent*> Grid;

	UFUNCTION(BlueprintPure, Meta=(WorldContext="WorldContextObject"))
	static FRobotArmGridCoordinate GetGridCoordinate(const UObject* WorldContextObject, const FVector& WorldLocation);

	static ThisClass* Get(const UObject* WorldContextObject);
};
