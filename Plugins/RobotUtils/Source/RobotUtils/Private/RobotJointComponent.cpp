// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotJointComponent.h"

#include "RobotUtilsFunctionLibrary.h"
#include "Materials/Material.h"
#include "Materials/MaterialRenderProxy.h"

// Sets default values for this component's properties
URobotJointComponent::URobotJointComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DrawJoint = false;
	DrawSize = 16.f;
	DrawOffset.SetLocation(FVector(0.1f, 0.f, 0.f));
	Rotation = 0.f;
	RotationInterpSpeed = 50.f;
	RotationStopThreshold = 0.1f;

	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;

	bVisibleInReflectionCaptures = false;
	bVisibleInRayTracing = false;
	bVisibleInRealTimeSkyCaptures = false;
}

void URobotJointComponent::InitializeComponent()
{
	Super::InitializeComponent();

	StartingRelativeTransform = GetRelativeTransform();
}

bool URobotJointComponent::MakeChain(USceneComponent* ChainTip, FRobotChain& OutChain, TArray<USceneComponent*>& OutJoints) const
{
	return URobotUtilsFunctionLibrary::MakeChainFromComponents(this, ChainTip, OutChain, OutJoints);
}

bool URobotJointComponent::GetJointRotations(USceneComponent* ChainTip, FRobotJointArray& Rotations) const
{
	Rotations.Rotations.Empty();
	TArray<USceneComponent*> Joints;
	FRobotChain Chain;

	if (MakeChain(ChainTip, Chain, Joints) == false)
	{
		return false;
	}

	for (USceneComponent* JointComponent : Joints)
	{
		if (URobotJointComponent* JointJointComponent = Cast<URobotJointComponent>(JointComponent))
		{
			if (JointJointComponent->Joint.Type == ERobotJointType::Fixed)
			{
				continue;
			}

			Rotations.Rotations.Add(FMath::DegreesToRadians(JointJointComponent->TargetRotation));
		}
	}

	return true;
}

bool URobotJointComponent::ApplyJointRotations(USceneComponent* ChainTip, const FRobotJointArray& Rotations)
{
	TArray<USceneComponent*> Joints;
	FRobotChain Chain;

	if (MakeChain(ChainTip, Chain, Joints) == false)
	{
		return false;
	}

	int32 i = 0;
	for (USceneComponent* JointComponent : Joints)
	{
		if (URobotJointComponent* JointJointComponent = Cast<URobotJointComponent>(JointComponent))
		{
			if (JointJointComponent->Joint.Type == ERobotJointType::Fixed)
			{
				continue;
			}

			check(Rotations.Rotations.IsValidIndex(i));

			JointJointComponent->SetTargetRotation(FMath::RadiansToDegrees(Rotations.Rotations[i]));
			i++;
		}
	}

	return true;
}

bool URobotJointComponent::SolveIK(const FSolveIKOptions& Options, const FRobotChain& Chain, TArray<USceneComponent*> Joints, const FTransform& DesiredEffectorTransform, FSolveIKResult& Result, bool bApplyIK)
{
	if (URobotUtilsFunctionLibrary::SolveIK(Options, Chain, DesiredEffectorTransform, Result) == false)
	{
		return false;
	}

	if (bApplyIK)
	{
		Joints.RemoveAll([](USceneComponent* Component)
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

		TArray<FRotator> JointRotations = URobotUtilsFunctionLibrary::GetJointRotations(Chain, Result.JointArray);
		for (int32 i = 0; i < JointRotations.Num(); ++i)
		{
			USceneComponent* JointComponent = Joints[i];

			if (URobotJointComponent* CastedJointComponent = Cast<URobotJointComponent>(JointComponent))
			{
				CastedJointComponent->SetTargetRotation(FMath::RadiansToDegrees(Result.JointArray.Rotations[i]));
			}
		}
	}

	return true;
}

void URobotJointComponent::SetTargetRotation(float InTargetRotation)
{
	TargetRotation = InTargetRotation;
}

bool URobotJointComponent::IsWithinRotationThreshold() const
{
	return FMath::IsNearlyEqual(Rotation, TargetRotation, RotationStopThreshold);
}

void URobotJointComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsWithinRotationThreshold())
	{
		float NewRotationDelta = (FMath::FInterpTo(Rotation, TargetRotation, DeltaTime, RotationInterpSpeed) - Rotation) / DeltaTime;

		NewRotationDelta = FMath::Clamp(NewRotationDelta, -Joint.MaxSpeed, Joint.MaxSpeed) * DeltaTime;
		Rotation += NewRotationDelta;
		
		// Now update the relative rotation
		const FVector Axis = URobotUtilsFunctionLibrary::GetJointTypeAxis(Joint.Type);
		const FQuat NewJointRotation = FQuat(Axis, FMath::DegreesToRadians(Rotation));

		SetRelativeRotation(NewJointRotation);
		OnRobotJointRotate.Broadcast(this, DeltaTime, NewRotationDelta);
	}
	else
	{
		OnRobotJointRotate.Broadcast(this, DeltaTime, 0.f);
	}
}

FPrimitiveSceneProxy* URobotJointComponent::CreateSceneProxy()
{
	return new FRobotJointSceneProxy(this);
}

FBoxSphereBounds URobotJointComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	const FVector BoxExtent(DrawSize);
	return FBoxSphereBounds(LocalToWorld.GetLocation(), BoxExtent, DrawSize);
}

SIZE_T FRobotJointSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}


FRobotJointSceneProxy::FRobotJointSceneProxy(const URobotJointComponent* InComponent)
	: FPrimitiveSceneProxy(InComponent)
	, VertexFactory(GetScene().GetFeatureLevel(), "FRobotJointSceneProxy")
	, RobotJointComponent(InComponent)
{
	DrawJoint = false;
	if (InComponent != nullptr)
	{
		Joint = InComponent->Joint;
		DrawJoint = InComponent->DrawJoint;
		DrawSize = InComponent->DrawSize;
		DrawOffset = InComponent->DrawOffset;
		JointRotationOffset = InComponent->JointRotationOffset;
		StartingRelativeTransform = InComponent->StartingRelativeTransform;
		if (InComponent->GetAttachParent())
		{
			ParentTransform = InComponent->GetAttachParent()->GetComponentTransform();
		}
		else
		{
			ParentTransform = FTransform::Identity;
		}
	}

	TArray<FDynamicMeshVertex> OutVerts;
	float Angle = (Joint.MinJointLimit + Joint.MaxJointLimit) * (PI / 360);
	Angle = FMath::Min(Angle, PI * 2.f);
	BuildConeVerts(0.f, Angle, DrawSize, 0.f, 32, OutVerts, IndexBuffer.Indices);

	VertexBuffers.InitFromDynamicVertex(&VertexFactory, OutVerts);

	// Enqueue initialization of render resource
	BeginInitResource(&IndexBuffer);
}

FRobotJointSceneProxy::~FRobotJointSceneProxy()
{
	VertexBuffers.PositionVertexBuffer.ReleaseResource();
	VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
	VertexBuffers.ColorVertexBuffer.ReleaseResource();
	IndexBuffer.ReleaseResource();
	VertexFactory.ReleaseResource();
}

void FRobotJointSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	if (RobotJointComponent == nullptr || DrawJoint == false)
	{
		return;
	}

	const FMatrix AxisRotMatrix = URobotUtilsFunctionLibrary::GetJointTypeAxis(Joint.Type).ToOrientationQuat().ToMatrix();
	const FMatrix JointMinMaxMat = FRotator((Joint.MaxJointLimit - Joint.MinJointLimit) / 2.f + JointRotationOffset, 0.f, 0.f).Quaternion().ToMatrix();

	FMatrix LocalMatrix = DrawOffset.ToMatrixWithScale() * JointMinMaxMat * FVector(0.f, 1.f, 0.f).ToOrientationQuat().ToMatrix() * AxisRotMatrix;

	FMatrix EffectiveLocalToWorld; 
	
	const auto WorldType = RobotJointComponent->GetWorld()->WorldType;
	if (WorldType == EWorldType::Game || WorldType == EWorldType::PIE)
	{
		EffectiveLocalToWorld = LocalMatrix * StartingRelativeTransform.ToMatrixWithScale() * ParentTransform.ToMatrixWithScale();
	}
	else
	{
		EffectiveLocalToWorld = LocalMatrix * GetLocalToWorld();
	}

	FColor RenderColor = FColor::White;

	switch (Joint.Type)
	{
	case ERobotJointType::RotX:
		RenderColor = FColor::Red;
		break;

	case ERobotJointType::RotY:
		RenderColor = FColor::Green;
		break;

	case ERobotJointType::RotZ:
		RenderColor = FColor::Blue;
		break;

	default:
		break;
	}

	auto ArrowMaterialRenderProxy = new FColoredMaterialRenderProxy(
		GEngine->ArrowMaterial->GetRenderProxy(),
		RenderColor,
		"GizmoColor"
	);

	Collector.RegisterOneFrameMaterialProxy(ArrowMaterialRenderProxy);

	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];

			// Draw the mesh.
			FMeshBatch& Mesh = Collector.AllocateMesh();
			FMeshBatchElement& BatchElement = Mesh.Elements[0];
			BatchElement.IndexBuffer = &IndexBuffer;
			Mesh.bWireframe = false;
			Mesh.VertexFactory = &VertexFactory;
			Mesh.MaterialRenderProxy = ArrowMaterialRenderProxy;

			FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
			DynamicPrimitiveUniformBuffer.Set(Collector.GetRHICommandList(), EffectiveLocalToWorld, EffectiveLocalToWorld, GetBounds(), GetLocalBounds(), true, false, AlwaysHasVelocity());
			BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

			BatchElement.FirstIndex = 0;
			BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
			BatchElement.MinVertexIndex = 0;
			BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
			Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
			Mesh.Type = PT_TriangleList;
			Mesh.DepthPriorityGroup = SDPG_World;
			Mesh.bCanApplyViewModeOverrides = false;
			Collector.AddMesh(ViewIndex, Mesh);
		}
	}
}

FPrimitiveViewRelevance FRobotJointSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance ViewRelevance;
	ViewRelevance.bDrawRelevance = IsShown(View);
	ViewRelevance.bDynamicRelevance = true;
	// ideally the TranslucencyRelevance should be filled out by the material, here we do it conservative
	ViewRelevance.bShadowRelevance = IsShadowCast(View);
	ViewRelevance.bEditorPrimitiveRelevance = UseEditorCompositing(View);
	ViewRelevance.bVelocityRelevance = DrawsVelocity() && ViewRelevance.bOpaque && ViewRelevance.bRenderInMainPass;
	return ViewRelevance;
}

uint32 FRobotJointSceneProxy::GetMemoryFootprint(void) const
{
	return(sizeof(*this) + GetAllocatedSize());
}

uint32 FRobotJointSceneProxy::GetAllocatedSize(void) const
{
	return FPrimitiveSceneProxy::GetAllocatedSize();
}

void FRobotJointSceneProxy::OnTransformChanged(FRHICommandListBase& RHICmdList)
{
	if (RobotJointComponent != nullptr)
	{
		StartingRelativeTransform = RobotJointComponent->StartingRelativeTransform;
		if (RobotJointComponent->GetAttachParent())
		{
			ParentTransform = RobotJointComponent->GetAttachParent()->GetComponentTransform();
		}
		else
		{
			ParentTransform = FTransform::Identity;
		}
	}
}
