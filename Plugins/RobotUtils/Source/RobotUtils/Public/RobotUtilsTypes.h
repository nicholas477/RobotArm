// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RobotUtilsTypes.generated.h"

namespace KDL
{
	class Frame;
	class Vector;
	class Joint;
	class Segment;
	class Chain;
	class JntArray;
}

UENUM(BlueprintType)
enum class ERobotJointType : uint8
{
	RotAxis,
	RotX,
	RotY,
	RotZ,
	TransAxis,
	TransX,
	TransY,
	TransZ,
	Fixed,
	None = Fixed
};

USTRUCT(BlueprintType)
struct FRobotJoint
{
	GENERATED_BODY()

	FRobotJoint();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	ERobotJointType Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	double Scale;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	double Offset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	double Inertia;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	double Damping;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	double Stiffness;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	FVector Axis;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint")
	FVector Origin;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint", meta = (Units = "Degrees"))
	double MinJointLimit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint", meta = (Units = "Degrees"))
	double MaxJointLimit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint", meta = (Units = "DegreesPerSecond"))
	double MaxSpeed;

	void MakeKDLJoint(KDL::Joint& OutJoint) const;
	static FRobotJoint FromKDLJoint(const KDL::Joint& Joint);
};

USTRUCT(BlueprintType)
struct FRobotSegment
{
	GENERATED_BODY()

	FRobotSegment();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Segment")
	FString Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Segment")
	FRobotJoint Joint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Segment")
	FTransform Tip;

	void MakeKDLSegment(KDL::Segment& OutSegment) const;
	static FRobotSegment FromKDLSegment(const KDL::Segment& Segment);
};

USTRUCT(BlueprintType)
struct FRobotJointArray
{
	GENERATED_BODY()

	// An array of rotations around the joint axes
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint Array")
	TArray<double> Rotations;

	void MakeKDLJointArray(KDL::JntArray& OutArray) const;
	static FRobotJointArray FromKDLJntArray(const KDL::JntArray& Array);
};

USTRUCT(BlueprintType)
struct FRobotChain
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Chain")
	TArray<FRobotSegment> Segments;

	void GetJointMinMaxRotations(FRobotJointArray& MinRotations, FRobotJointArray& MaxRotations) const;

	void MakeKDLChain(KDL::Chain& OutChain) const;
	static FRobotChain FromKDLChain(const KDL::Chain& Chain);
};