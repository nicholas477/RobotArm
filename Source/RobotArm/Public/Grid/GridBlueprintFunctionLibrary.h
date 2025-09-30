// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Grid/GridTypes.h"
#include "GridBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTARM_API UGridBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static FRobotArmGridCoordinate ConvertToGridCoordinate(const FWorldGridSettings& WorldGridSettings, const FVector& WorldLocation) { return WorldGridSettings.ConvertToGridCoordinate(WorldLocation); }

	UFUNCTION(BlueprintPure)
	static FVector ConvertToWorldCoordinate(const FWorldGridSettings& WorldGridSettings, const FRobotArmGridCoordinate& GridCoordinate) { return WorldGridSettings.ConvertToWorldCoordinate(GridCoordinate); }

	UFUNCTION(BlueprintPure)
	static FString ToString(const FRobotArmGridCoordinate& GridCoordinate) { return GridCoordinate.ToString(); }
};
