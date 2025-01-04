// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotUtilsTypes.h"

#include "RobotUtilsKDL.h"
#include "RobotUtilsFunctionLibrary.h"

FRobotJoint::FRobotJoint()
{
	Name = "NoName";
	Type = ERobotJointType::Fixed;
	Scale = 1.0;
	Offset = 0.0;
	Inertia = 0.0;
	Damping = 0.0;
	Stiffness = 0.0;
	MinJointLimit = 180.0;
	MaxJointLimit = 180.0;
	MaxSpeed = 180.0;
}

void FRobotJoint::MakeKDLJoint(KDL::Joint& OutJoint) const
{
	OutJoint = KDL::Joint(TCHAR_TO_UTF8(*Name), static_cast<KDL::Joint::JointType>(Type), Scale, Offset, Inertia, Damping, Stiffness);
}

FRobotJoint FRobotJoint::FromKDLJoint(const KDL::Joint& Joint)
{
	FRobotJoint OutJoint;
	OutJoint.Name = UTF8_TO_TCHAR(Joint.getName().c_str());
	OutJoint.Type = static_cast<ERobotJointType>(Joint.getType());
	OutJoint.Scale = Joint.getScale();
	OutJoint.Offset = Joint.getOffset();
	OutJoint.Inertia = Joint.getInertia();
	OutJoint.Damping = Joint.getDamping();
	OutJoint.Stiffness = Joint.getStiffness();
	OutJoint.Axis = URobotUtilsFunctionLibrary::KDLVectorToVector(Joint.getAxis());
	return OutJoint;
}

FRobotSegment::FRobotSegment()
{
	Joint = FRobotJoint();
	Tip = FTransform::Identity;
}

void FRobotSegment::MakeKDLSegment(KDL::Segment& OutSegment) const
{
	KDL::Joint OutJoint;
	Joint.MakeKDLJoint(OutJoint);

	KDL::Frame OutTip;
	URobotUtilsFunctionLibrary::TransformToKDLFrame(Tip, OutTip);

	OutSegment = KDL::Segment(TCHAR_TO_UTF8(*Name), OutJoint, OutTip);
}

FRobotSegment FRobotSegment::FromKDLSegment(const KDL::Segment& Segment)
{
	//UE_LOG(LogTemp, Warning, TEXT("KDL: FRobotSegment::FromKDLSegment(const KDL::Segment& Segment)"));
	FRobotSegment OutSegment;
	OutSegment.Name = UTF8_TO_TCHAR(Segment.getName().c_str());
	OutSegment.Joint.FromKDLJoint(Segment.getJoint());
	OutSegment.Tip = URobotUtilsFunctionLibrary::KDLFrameToTransform(Segment.getFrameToTipZero());
	return OutSegment;
}

void FRobotChain::GetJointMinMaxRotations(FRobotJointArray& MinRotations, FRobotJointArray& MaxRotations) const
{
}

void FRobotChain::MakeKDLChain(KDL::Chain& OutChain) const
{
	//UE_LOG(LogTemp, Warning, TEXT("KDL: FRobotChain::MakeKDLChain(KDL::Chain& OutChain) const"));
	OutChain = KDL::Chain();
	for (const FRobotSegment& Segment : Segments)
	{
		KDL::Segment NewSegment;
		Segment.MakeKDLSegment(NewSegment);
		OutChain.addSegment(NewSegment);
	}
}

FRobotChain FRobotChain::FromKDLChain(const KDL::Chain& Chain)
{
	//UE_LOG(LogTemp, Warning, TEXT("KDL: FRobotChain::FromKDLChain(const KDL::Chain& Chain)"));

	FRobotChain OutChain;

	for (const KDL::Segment& segment : Chain.segments)
	{
		OutChain.Segments.Add(FRobotSegment::FromKDLSegment(segment));
	}

	return OutChain;
}

void FRobotJointArray::MakeKDLJointArray(KDL::JntArray& OutArray) const
{
	//UE_LOG(LogTemp, Warning, TEXT("KDL:  FRobotJointArray::MakeKDLJointArray(KDL::JntArray& OutArray) const"));
	OutArray = KDL::JntArray(Rotations.Num());
	
	for (int32 i = 0; i < Rotations.Num(); ++i)
	{
		OutArray(i) = Rotations[i];
	}
}

FRobotJointArray FRobotJointArray::FromKDLJntArray(const KDL::JntArray& Array)
{
	//UE_LOG(LogTemp, Warning, TEXT("KDL:  FRobotJointArray::FromKDLJntArray(const KDL::JntArray& Array)"));
	FRobotJointArray OutArray;

	for (unsigned int i = 0; i < Array.rows(); ++i)
	{
		OutArray.Rotations.Add(Array(i));
	}

	return OutArray;
}
