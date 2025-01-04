// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotUtilsEditorPropertyTypes.h"

#include "RobotUtilsEditor.h"
#include "PropertyEditorModule.h"
#include "IDetailCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailWidgetRow.h"
#include "IDetailChildrenBuilder.h"

#include "RobotUtilsTypes.h"
#include "RobotUtilsKDL.h"

#define LOCTEXT_NAMESPACE "FRobotUtilsEditorModule"

//static void AddDetailRow(IDetailChildrenBuilder& StructBuilder, const FText& RowName, const FString& Name, const FString& Value)
//{
//	StructBuilder.AddCustomRow(RowName)
//		.NameContent()
//		[
//			SNew(STextBlock)
//				.Text(FText::FromString(Name))
//				.Font(IDetailLayoutBuilder::GetDetailFont())
//		]
//		.ValueContent()
//		[
//			SNew(STextBlock)
//				.Text(FText::FromString(Value))
//				.Font(IDetailLayoutBuilder::GetDetailFont())
//		];
//}
//
//static void CreateJointDetails(IDetailChildrenBuilder& StructBuilder, const KDL::Joint& Joint)
//{
//	//std::string name;
//	//Joint::JointType type;
//	//double scale;
//	//double offset;
//	//double inertia;
//	//double damping;
//	//double stiffness;
//
//	//// variables for RotAxis joint
//	//Vector axis, origin;
//
//	AddDetailRow(StructBuilder, LOCTEXT("JointName", "Name"), "Name", UTF8_TO_TCHAR(Joint.getName().c_str()));
//	AddDetailRow(StructBuilder, LOCTEXT("JointType", "Type"), "Type", UTF8_TO_TCHAR(Joint.getTypeName().c_str()));
//	AddDetailRow(StructBuilder, LOCTEXT("JointScale", "Scale"), "Scale", FString::SanitizeFloat(Joint.getScale()));
//	AddDetailRow(StructBuilder, LOCTEXT("JointOffset", "Offset"), "Offset", FString::SanitizeFloat(Joint.getOffset()));
//	AddDetailRow(StructBuilder, LOCTEXT("JointInertia", "Inertia"), "Inertia", FString::SanitizeFloat(Joint.getInertia()));
//	AddDetailRow(StructBuilder, LOCTEXT("JointDamping", "Damping"), "Damping", FString::SanitizeFloat(Joint.getDamping()));
//	AddDetailRow(StructBuilder, LOCTEXT("JointStiffness", "Stiffness"), "Stiffness", FString::SanitizeFloat(Joint.getStiffness()));
//
//	FVector Axis;
//	Axis.X = Joint.getAxis().x();
//	Axis.Y = Joint.getAxis().y();
//	Axis.Z = Joint.getAxis().z();
//
//	AddDetailRow(StructBuilder, LOCTEXT("JointAxis", "Axis"), "Axis", Axis.ToString());
//
//	FVector Origin;
//	Origin.X = Joint.getOrigin().x();
//	Origin.Y = Joint.getOrigin().y();
//	Origin.Z = Joint.getOrigin().z();
//
//	AddDetailRow(StructBuilder, LOCTEXT("JointOrigin", "Origin"), "Origin", Origin.ToString());
//}
//
//static void CreateSegmentDetails(IDetailChildrenBuilder& StructBuilder, const KDL::Segment& Segment)
//{
//	AddDetailRow(StructBuilder, LOCTEXT("SegmentName", "Name"), "Name", UTF8_TO_TCHAR(Segment.getName().c_str()));
//	CreateJointDetails(StructBuilder, Segment.getJoint());
//}
//
//void FRobotJointDetails::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
//{
//	// create struct header
//	HeaderRow.NameContent()
//	[
//		StructPropertyHandle->CreatePropertyNameWidget()
//	];
//}
//
//void FRobotJointDetails::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
//{
//	TArray<void*> RawDataPtrs;
//	StructPropertyHandle->AccessRawData(RawDataPtrs);
//	for (void* RawPtr : RawDataPtrs)
//	{
//		FRobotJoint& Joint = *static_cast<FRobotJoint*>(RawPtr);
//		if (!Joint.Joint.IsValid())
//		{
//			continue;
//		}
//
//		CreateJointDetails(StructBuilder, *Joint);
//	}
//}
//
//void FRobotSegmentDetails::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
//{
//	// create struct header
//	HeaderRow.NameContent()
//	[
//		StructPropertyHandle->CreatePropertyNameWidget()
//	];
//}
//
//void FRobotSegmentDetails::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
//{
//	TArray<void*> RawDataPtrs;
//	StructPropertyHandle->AccessRawData(RawDataPtrs);
//	for (void* RawPtr : RawDataPtrs)
//	{
//		FRobotSegment& Segment = *static_cast<FRobotSegment*>(RawPtr);
//		if (!Segment.Segment.IsValid())
//		{
//			continue;
//		}
//
//        //std::string name;
//        //Joint joint;
//        //RigidBodyInertia I;
//        //Frame f_tip;
//
//		//StructBuilder.AddChildStructureProperty
//		CreateSegmentDetails(StructBuilder, *Segment);
//	}
//}

#undef LOCTEXT_NAMESPACE
