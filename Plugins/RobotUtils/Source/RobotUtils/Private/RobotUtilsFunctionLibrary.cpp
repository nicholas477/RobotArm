// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotUtilsFunctionLibrary.h"

#include "RobotUtils.h"
#include "RobotUtilsKDL.h"
#include "DrawDebugHelpers.h"
#include "RobotJointComponent.h"
#include "Algo/Reverse.h"
#include "Async/Async.h"

UE_DISABLE_OPTIMIZATION
bool URobotUtilsFunctionLibrary::GetJointRotation(const FRobotJoint& Joint, const FRobotJointArray& JointArray, int32 Index, FRotator& OutRotator)
{
	if (Index >= JointArray.Rotations.Num())
	{
		return false;
	}

	FVector Axis = GetJointTypeAxis(Joint.Type);
	OutRotator = FQuat(Axis, JointArray.Rotations[Index]).Rotator();
	return true;
}

bool URobotUtilsFunctionLibrary::MakeChainFromComponents(const USceneComponent* ChainBase, USceneComponent* ChainTip, FRobotChain& OutChain, TArray<USceneComponent*>& OutJoints)
{
	if (ChainTip == nullptr)
	{
		UE_LOG(LogRobotUtils, Warning, TEXT("ChainTip was null"));
		return false;
	}

	OutJoints.Empty();

	{
		USceneComponent* CurrentSegment = ChainTip;
		while (CurrentSegment != nullptr && CurrentSegment != ChainBase)
		{
			OutJoints.Push(CurrentSegment);

			CurrentSegment = CurrentSegment->GetAttachParent();
		}

		if (CurrentSegment == ChainBase && ChainBase != nullptr)
		{
			OutJoints.Push(CurrentSegment);
		}
	}

	if (OutJoints.Num() < 2)
	{
		UE_LOG(LogRobotUtils, Warning, TEXT("Did not find more than 2 components in chain"));
		return false;
	}

	// The component hierarchy was walked from child to parent so reverse it
	Algo::Reverse(OutJoints);

	OutChain = FRobotChain();

	// Walk the chain in reverse and turn it into a FRobotChain
	for (int32 i = 0; i < OutJoints.Num() - 1; ++i)
	{
		USceneComponent* CurrentSegment = OutJoints[i];

		FRobotSegment NewSegment;
		NewSegment.Name = CurrentSegment->GetName();
		if (URobotJointComponent* JointComponent = Cast<URobotJointComponent>(CurrentSegment))
		{
			NewSegment.Joint = JointComponent->Joint;
		}
		else
		{
			NewSegment.Joint.Name = "Joint: " + CurrentSegment->GetName();
			NewSegment.Joint.Type = ERobotJointType::Fixed;
		}
		NewSegment.Tip = OutJoints[i + 1]->GetRelativeTransform();

		OutChain.Segments.Add(NewSegment);
	}

	return true;
}

void URobotUtilsFunctionLibrary::DebugDrawChain(const UObject* WorldContextObject, const FTransform& ChainWorldTransform, const FRobotChain& Chain)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		FTransform CurrentTransform = ChainWorldTransform;
		for (int32 i = 0; i < Chain.Segments.Num(); ++i)
		{
			KDL::Segment Segment;
			Chain.Segments[i].MakeKDLSegment(Segment);

			const FTransform SegmentBaseTransform = KDLFrameToTransform(Segment.getFrameToTip()) * CurrentTransform;

			const FString SegmentName = UTF8_TO_TCHAR(Segment.getName().c_str());

			DrawDebugString(World, CurrentTransform.GetLocation(), SegmentName, nullptr, FColor::Green, -1.f, false, 1.f);
			DrawDebugLine(World, CurrentTransform.GetLocation(), SegmentBaseTransform.GetLocation(), FColor::Red, false, -1, SDPG_Foreground);

			CurrentTransform = SegmentBaseTransform;
		}
	}
}

bool URobotUtilsFunctionLibrary::SolveIK(const FSolveIKOptions& Options, const FRobotChain& Chain, const FTransform& DesiredEffectorTransform, FSolveIKResult& Result)
{
	SCOPED_NAMED_EVENT(URobotUtilsFunctionLibrary_SolveIK, FColor::Green);
	DECLARE_SCOPE_CYCLE_COUNTER(TEXT("URobotUtilsFunctionLibrary::SolveIK"), STAT_RobotUtils_SolveIK, STATGROUP_RobotUtils);

	KDL::Chain chain;
	Chain.MakeKDLChain(chain);

	KDL::ChainFkSolverPos_recursive pos_solver(chain);
	KDL::ChainIkSolverVel_pinv vel_solver(chain, Options.VelocityEpsilon);

	KDL::JntArray q_init(chain.getNrOfJoints());
	if (chain.getNrOfJoints() == Options.InitialRotations.Rotations.Num())
	{
		for (int32 i = 0; i < Options.InitialRotations.Rotations.Num(); ++i)
		{
			q_init(i) = Options.InitialRotations.Rotations[i];
		}
	}

	KDL::JntArray solution(chain.getNrOfJoints());
	KDL::Frame desired_pose;
	TransformToKDLFrame(DesiredEffectorTransform, desired_pose);

	if (Options.bLimitJoints)
	{
		FRobotJointArray MinRotationLimit, MaxRotationLimit;
		GetJointLimitsFromChain(Chain, MinRotationLimit, MaxRotationLimit);

		KDL::JntArray KDLMinRotationLimit, KDLMaxRotationLimit;
		MinRotationLimit.MakeKDLJointArray(KDLMinRotationLimit);
		MaxRotationLimit.MakeKDLJointArray(KDLMaxRotationLimit);

		KDL::ChainIkSolverPos_NR_JL IKSolver(chain, KDLMinRotationLimit, KDLMaxRotationLimit, pos_solver, vel_solver, Options.MaxIterations, Options.PositionEpsilon);

		SCOPED_NAMED_EVENT(URobotUtilsFunctionLibrary_SolveIK_IK_CartToJnt, FColor::Green);
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("URobotUtilsFunctionLibrary::SolveIK IK::CartToJnt"), STAT_RobotUtils_SolveIK_IK_CartToJnt, STATGROUP_RobotUtils);
		Result.Result = IKSolver.CartToJnt(q_init, desired_pose, solution);
		Result.ErrorString = UTF8_TO_TCHAR(IKSolver.strError(Result.Result));
	}
	else
	{
		KDL::ChainIkSolverPos_NR IKSolver(chain, pos_solver, vel_solver, Options.MaxIterations, Options.PositionEpsilon);

		SCOPED_NAMED_EVENT(URobotUtilsFunctionLibrary_SolveIK_IK_CartToJnt, FColor::Green);
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("URobotUtilsFunctionLibrary::SolveIK IK::CartToJnt"), STAT_RobotUtils_SolveIK_IK_CartToJnt, STATGROUP_RobotUtils);
		Result.Result = IKSolver.CartToJnt(q_init, desired_pose, solution);
		Result.ErrorString = UTF8_TO_TCHAR(IKSolver.strError(Result.Result));
	}

	Result.JointArray = FRobotJointArray::FromKDLJntArray(solution);

	KDL::Twist TargetTwist;
	TwistToKDLTwist(Options.IVKOptions.TargetTwist, TargetTwist);

	KDL::JntArray vel_arr(chain.getNrOfJoints());

	{
		SCOPED_NAMED_EVENT(URobotUtilsFunctionLibrary_SolveIK_Vel_CartToJnt, FColor::Green);
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("URobotUtilsFunctionLibrary::SolveIK Vel::CartToJnt"), STAT_RobotUtils_SolveIK_Vel_CartToJnt, STATGROUP_RobotUtils);
		Result.VelocityResult = vel_solver.CartToJnt(q_init, TargetTwist, vel_arr);
	}

	Result.VelocityErrorString = UTF8_TO_TCHAR(vel_solver.strError(Result.VelocityResult));
	Result.VelocityJointArray = FRobotJointArray::FromKDLJntArray(vel_arr);

	if (Result.Result >= 0 && Result.VelocityResult >= 0)
	{
		Result.bWasSuccessful = true;
		return true;
	}

	Result.bWasSuccessful = false;
	return false;
}

TArray<FRotator> URobotUtilsFunctionLibrary::GetJointRotations(const FRobotChain& Chain, const FRobotJointArray& JointArray)
{
	SCOPED_NAMED_EVENT(URobotUtilsFunctionLibrary_GetJointRotations, FColor::Green);
	TArray<FRotator> Rotations;
	KDL::Chain chain;
	Chain.MakeKDLChain(chain);

	TArray<FRobotJoint> Joints = GetJointsFromChain(Chain, false);

	for (int32 i = 0; i < Joints.Num(); ++i)
	{
		FRotator JointRot;
		FRobotJoint Joint = Joints[i];
		bool Res = GetJointRotation(Joint, JointArray, i, JointRot);
		check(Res != false);

		Rotations.Add(JointRot);
	}
	return Rotations;
}

FVector URobotUtilsFunctionLibrary::GetJointTypeAxis(ERobotJointType JointType)
{
	switch (JointType)
	{
	case ERobotJointType::RotX:
		return FVector::XAxisVector;
	case ERobotJointType::RotY:
		return FVector::YAxisVector;
	case ERobotJointType::RotZ:
		return FVector::ZAxisVector;

	default:
		return FVector::Zero();
	}
}

void URobotUtilsFunctionLibrary::TransformToKDLFrame(const FTransform& Transform, KDL::Frame& OutFrame)
{
	OutFrame.p = KDL::Vector(Transform.GetLocation().X, Transform.GetLocation().Y, Transform.GetLocation().Z);
	const FQuat TransQuat = Transform.GetRotation();

	OutFrame.M = KDL::Rotation::Quaternion(TransQuat.X, TransQuat.Y, TransQuat.Z, TransQuat.W);
}

FTransform URobotUtilsFunctionLibrary::KDLFrameToTransform(const KDL::Frame& Frame)
{
	FTransform OutTransform = FTransform::Identity;

	OutTransform.SetLocation(FVector(Frame.p.x(), Frame.p.y(), Frame.p.z()));

	FQuat Rot;
	Frame.M.GetQuaternion(Rot.X, Rot.Y, Rot.Z, Rot.W);

	OutTransform.SetRotation(Rot);

	return OutTransform;
}

void URobotUtilsFunctionLibrary::VectorToKDLVector(const FVector& Vector, KDL::Vector& OutVector)
{
	OutVector = KDL::Vector(Vector.X, Vector.Y, Vector.Z);
}

FVector URobotUtilsFunctionLibrary::KDLVectorToVector(const KDL::Vector& Vector)
{
	return FVector(Vector.x(), Vector.y(), Vector.z());
}

void URobotUtilsFunctionLibrary::TwistToKDLTwist(const FRobotTwist& Twist, KDL::Twist& OutTwist)
{
	VectorToKDLVector(Twist.Velocity, OutTwist.vel);
	VectorToKDLVector(Twist.RotationalVelocity, OutTwist.rot);
}

FRobotTwist URobotUtilsFunctionLibrary::KDLTwistToTwist(const KDL::Twist& Twist)
{
	return FRobotTwist{ KDLVectorToVector(Twist.vel), KDLVectorToVector(Twist.rot) };
}

TArray<FRobotJoint> URobotUtilsFunctionLibrary::GetJointsFromChain(const FRobotChain& Chain, bool bIncludeFixed)
{
	TArray<FRobotJoint> Joints;
	for (const FRobotSegment& Segment : Chain.Segments)
	{
		if (!bIncludeFixed && Segment.Joint.Type == ERobotJointType::Fixed)
		{
			continue;
		}

		Joints.Add(Segment.Joint);
	}
	return Joints;
}

void URobotUtilsFunctionLibrary::GetJointLimitsFromChain(const FRobotChain& Chain, FRobotJointArray& OutMin, FRobotJointArray& OutMax)
{
	OutMin.Rotations.Reset();
	OutMax.Rotations.Reset();

	for (const FRobotSegment& Segment : Chain.Segments)
	{
		if (Segment.Joint.Type != ERobotJointType::Fixed)
		{
			OutMin.Rotations.Add(-FMath::DegreesToRadians(Segment.Joint.MinJointLimit));
			OutMax.Rotations.Add(FMath::DegreesToRadians(Segment.Joint.MaxJointLimit));
		}
	}
}

TArray<USceneComponent*> URobotUtilsFunctionLibrary::GetMoveableJointsFromChain(const TArray<USceneComponent*>& Chain)
{
	TArray<USceneComponent*> OutComponents = Chain;
	OutComponents.RemoveAll([](USceneComponent* Component)
	{
		URobotJointComponent* JointComponent = Cast<URobotJointComponent>(Component);
		if (JointComponent)
		{
			return JointComponent->Joint.Type == ERobotJointType::Fixed;
		}
		else
		{
			return true;
		}
	});
	return OutComponents;
}

FSolveIVKOptions::FSolveIVKOptions()
{
	TargetTwist.Velocity = FVector(100.f);
	TargetTwist.RotationalVelocity = FVector(3.14159f);
}
UE_ENABLE_OPTIMIZATION