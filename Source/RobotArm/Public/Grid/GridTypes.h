// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridTypes.generated.h"

USTRUCT(BlueprintType)
struct ROBOTARM_API FRobotArmGridCoordinate
{
	GENERATED_BODY()

	FRobotArmGridCoordinate() = default;

	FRobotArmGridCoordinate(int32 X, int32 Y, int32 Z)
		: Coordinate(X, Y, Z)
	{

	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FIntVector Coordinate = FIntVector(0, 0, 0);

	bool operator==(const FRobotArmGridCoordinate& Other) const
	{
		return Coordinate == Other.Coordinate;
	}

	bool operator!=(const FRobotArmGridCoordinate& Other) const
	{
		return !(*this == Other);
	}

	FString ToString() const
	{
		return FString::Printf(TEXT("Robot Arm Grid Coordinate X: %d, Y: %d, Z: %d"), Coordinate.X, Coordinate.Y, Coordinate.Z);
	}
};

static uint32 GetTypeHash(const FRobotArmGridCoordinate& Thing)
{
	return GetTypeHash(Thing.Coordinate);
}

USTRUCT(BlueprintType)
struct ROBOTARM_API FWorldGridSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GridOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector GridSize = FVector(100.f, 100.f, 100.f);

	FRobotArmGridCoordinate ConvertToGridCoordinate(const FVector& WorldLocation) const
	{
		const FVector GridLocation = (WorldLocation - GridOffset) / GridSize;
		return FRobotArmGridCoordinate(FMath::Floor(GridLocation.X), FMath::Floor(GridLocation.Y), FMath::Floor(GridLocation.Z));
	}

	FVector ConvertToWorldCoordinate(const FRobotArmGridCoordinate& GridLocation) const
	{
		const FVector SizedGridLocation = FVector(GridLocation.Coordinate.X, GridLocation.Coordinate.Y, GridLocation.Coordinate.Z) * GridSize;
		return SizedGridLocation + GridOffset;
	}
};