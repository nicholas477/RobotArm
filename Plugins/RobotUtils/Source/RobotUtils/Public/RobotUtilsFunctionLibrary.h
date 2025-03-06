// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RobotUtilsTypes.h"
#include "RobotUtilsFunctionLibrary.generated.h"

// Inverse velocity kinematics solver options
// 
// Inverse kinematics is specifying a position and inversely solving joint rotations to match that position,
// so inverse velocity kinematics is specifying an end effector velocity and solving joint rotational velocities
// to match that end effector velocity
USTRUCT(BlueprintType)
struct FSolveIVKOptions
{
	GENERATED_BODY()

	// Target velocity of the end effector
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IVK Options")
	FRobotTwist TargetTwist;
};

USTRUCT(BlueprintType)
struct FSolveIKOptions
{
	GENERATED_BODY()

	// Number of iterations before the IK solver gives up and returns an error
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Options")
	int32 MaxIterations = 100;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Options")
	double PositionEpsilon = 0.01;
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Options")
	double VelocityEpsilon = 0.01;

	// Read the rotation limits from the joints and limit the IK rotations
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Options")
	bool bLimitJoints = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Options")
	FRobotJointArray InitialRotations;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Options")
	FSolveIVKOptions IVKOptions;
};

USTRUCT(BlueprintType)
struct FSolveIKResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Result")
	bool bWasSuccessful = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Result")
	FRobotJointArray JointArray;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Result")
	FRobotJointArray VelocityJointArray;

	// The result returned from the IK solver
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Result")
	int32 Result;

	// The result returned from the IK velocity solver
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Result")
	int32 VelocityResult;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Result")
	FString ErrorString;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Robot|IK Result")
	FString VelocityErrorString;
};

/**
 * 
 */
UCLASS()
class ROBOTUTILS_API URobotUtilsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Robot Utils|Chain", meta = (WorldContext = "WorldContextObject"))
	static bool GetJointRotation(const FRobotJoint& Joint, const FRobotJointArray& JointArray, int32 Index, FRotator& OutRotator);

	UFUNCTION(BlueprintPure, Category = "Robot Utils|Chain")
	static bool MakeChainFromComponents(const USceneComponent* ChainBase, USceneComponent* ChainTip, FRobotChain& OutChain, TArray<USceneComponent*>& OutJoints);

	UFUNCTION(BlueprintCallable, Category = "Robot Utils|Chain", meta=(WorldContext = "WorldContextObject"))
	static void DebugDrawChain(const UObject* WorldContextObject, const FTransform& ChainWorldTransform, const FRobotChain& Chain);

	UFUNCTION(BlueprintCallable, Category = "Robot Utils|Chain")
	static bool SolveIK(const FSolveIKOptions& Options, const FRobotChain& Chain, const FTransform& DesiredEffectorTransform, FSolveIKResult& Result);

	UFUNCTION(BlueprintPure, Category = "Robot Utils|Chain")
	static TArray<FRotator> GetJointRotations(const FRobotChain& Chain, const FRobotJointArray& JointArray);

	UFUNCTION(BlueprintPure, Category = "Robot Utils|Chain")
	static FVector GetJointTypeAxis(ERobotJointType JointType);

	static void TransformToKDLFrame(const FTransform& Transform, KDL::Frame& OutFrame);
	static FTransform KDLFrameToTransform(const KDL::Frame& Frame);

	static void VectorToKDLVector(const FVector& Vector, KDL::Vector& OutVector);
	static FVector KDLVectorToVector(const KDL::Vector& Vector);

	static void TwistToKDLTwist(const FRobotTwist& Twist, KDL::Twist& OutTwist);
	static FRobotTwist KDLTwistToTwist(const KDL::Twist& Twist);

	UFUNCTION(BlueprintPure, Category = "Robot Utils|Chain")
	static TArray<FRobotJoint> GetJointsFromChain(const FRobotChain& Chain, bool bIncludeFixed = false);

	UFUNCTION(BlueprintPure, Category = "Robot Utils|Chain")
	static void GetJointLimitsFromChain(const FRobotChain& Chain, FRobotJointArray& OutMin, FRobotJointArray& OutMax);

	// Removes all non-robot joint components and also all fixed robot joint components.
	UFUNCTION(BlueprintPure, Category = "Robot Utils|Chain")
	static TArray<USceneComponent*> GetMoveableJointsFromChain(const TArray<USceneComponent*>& Chain);
};
