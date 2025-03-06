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
	class Twist;
	class JntArrayVel;
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

// Represents the combination of translational velocity and rotational velocity applied at one point.
// The same as KDL::Twist
USTRUCT(BlueprintType)
struct FRobotTwist
{
	GENERATED_BODY()

	// Linear velocity
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Twist")
	FVector Velocity;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Twist")
	FVector RotationalVelocity;
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
struct FRobotJointVelocityArray
{
	GENERATED_BODY()

	// Velocity
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint Array")
	FRobotJointArray Q;

	// Acceleration
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|Joint Array")
	FRobotJointArray QDot;

	void MakeKDLJointVelocityArray(KDL::JntArrayVel& OutArray) const;
	static FRobotJointVelocityArray FromKDLJntVelocityArray(const KDL::JntArrayVel& Array);
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