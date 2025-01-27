// Fill out your copyright notice in the Description page of Project Settings.


#include "Rail/RailConnectionSceneProxy.h"

#include "Rail/RailConnectionComponent.h"
#include "MaterialDomain.h"
#include "Materials/Material.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialRenderProxy.h"

FRailConnectionSceneProxy::FRailConnectionSceneProxy(const URailConnectionComponent* InComponent)
	: FPrimitiveSceneProxy(InComponent)
	, VertexFactory(GetScene().GetFeatureLevel(), "FRailConnectionSceneProxy")
	, RailConnectionComponent(InComponent)
	, Color(InComponent->VisualizationColor)
{
	TArray<FDynamicMeshVertex> OutVerts;
	CreateLineData(OutVerts);
	bDrawArrow = InComponent->HasAnyCircularConnections();
	
	if (OutVerts.Num() > 0)
	{
		VertexBuffers.InitFromDynamicVertex(&VertexFactory, OutVerts);
	}

	// Enqueue initialization of render resource
	if (LineIndexBuffer.Indices.Num() > 0)
	{
		BeginInitResource(&LineIndexBuffer);
	}
}

FRailConnectionSceneProxy::~FRailConnectionSceneProxy()
{
	VertexBuffers.PositionVertexBuffer.ReleaseResource();
	VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
	VertexBuffers.ColorVertexBuffer.ReleaseResource();
	LineIndexBuffer.ReleaseResource();
	VertexFactory.ReleaseResource();
}

void FRailConnectionSceneProxy::GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const
{
	for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
	{
		if (VisibilityMap & (1 << ViewIndex))
		{
			const FSceneView* View = Views[ViewIndex];

			CreateLineBatch(ViewIndex, Collector);

			if (bDrawArrow)
			{
				DrawArrow(ViewIndex, Collector, View);
			}
		}
	}
}

FPrimitiveViewRelevance FRailConnectionSceneProxy::GetViewRelevance(const FSceneView* View) const
{
	FPrimitiveViewRelevance ViewRelevance;
	ViewRelevance.bDrawRelevance = IsShown(View);
	ViewRelevance.bDynamicRelevance = true;
	ViewRelevance.bRenderInMainPass = ShouldRenderInMainPass();
	// ideally the TranslucencyRelevance should be filled out by the material, here we do it conservative
	ViewRelevance.bShadowRelevance = IsShadowCast(View);
	ViewRelevance.bEditorPrimitiveRelevance = true;// UseEditorCompositing(View);
	ViewRelevance.bVelocityRelevance = DrawsVelocity() && ViewRelevance.bOpaque && ViewRelevance.bRenderInMainPass;
	return ViewRelevance;
}

uint32 FRailConnectionSceneProxy::GetMemoryFootprint(void) const
{
	return(sizeof(*this) + GetAllocatedSize());
}

uint32 FRailConnectionSceneProxy::GetAllocatedSize(void) const
{
	return FPrimitiveSceneProxy::GetAllocatedSize();
}

SIZE_T FRailConnectionSceneProxy::GetTypeHash() const
{
	static size_t UniquePointer;
	return reinterpret_cast<size_t>(&UniquePointer);
}

void FRailConnectionSceneProxy::CreateLineBatch(int32 ViewIndex, FMeshElementCollector& Collector) const
{
	if (!LineIndexBuffer.IsInitialized())
	{
		return;
	}

	auto WireframeMaterial = new FColoredMaterialRenderProxy(
		GEngine->LevelColorationUnlitMaterial->GetRenderProxy(),
		Color
	);

	Collector.RegisterOneFrameMaterialProxy(WireframeMaterial);

	FMeshBatch& Mesh = Collector.AllocateMesh();
	FMeshBatchElement& BatchElement = Mesh.Elements[0];
	BatchElement.IndexBuffer = &LineIndexBuffer;
	Mesh.bWireframe = false;
	Mesh.VertexFactory = &VertexFactory;
	Mesh.MaterialRenderProxy = WireframeMaterial;

	FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
	DynamicPrimitiveUniformBuffer.Set(Collector.GetRHICommandList(), GetLocalToWorld(), GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, false, AlwaysHasVelocity());
	BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

	BatchElement.FirstIndex = 0;
	BatchElement.NumPrimitives = LineIndexBuffer.Indices.Num() / 2;
	BatchElement.MinVertexIndex = 0;
	BatchElement.MaxVertexIndex = VertexBuffers.PositionVertexBuffer.GetNumVertices() - 1;
	Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
	Mesh.Type = PT_LineList;
	Mesh.DepthPriorityGroup = SDPG_World;
	Mesh.bCanApplyViewModeOverrides = false;
	Collector.AddMesh(ViewIndex, Mesh);
}

void FRailConnectionSceneProxy::DrawArrow(int32 ViewIndex, FMeshElementCollector& Collector, const FSceneView* View) const
{
	DrawDirectionalArrow(Collector.GetPDI(ViewIndex), GetLocalToWorld(), Color, 50.f, 8.f, SDPG_World);
}

void FRailConnectionSceneProxy::CreateLineData(TArray<FDynamicMeshVertex>& OutVerts)
{
	TArray<FVector> ConnectionPositions = RailConnectionComponent->GetConnectionRelativePositions();
	if (ConnectionPositions.Num() == 0)
	{
		return;
	}

	// Add vert 0
	const int32 VertZero = OutVerts.Num();
	{
		auto& NewVert = OutVerts.AddDefaulted_GetRef();
		NewVert.Position = FVector3f(0.f, 0.f, 0.f);
		NewVert.Color = FColor::White;
	}

	for (const FVector& ConnectionPosition : ConnectionPositions)
	{
		auto& NewVert = OutVerts.AddDefaulted_GetRef();
		NewVert.Position = FVector3f(ConnectionPosition);
		LineIndexBuffer.Indices.Add(VertZero);
		LineIndexBuffer.Indices.Add(OutVerts.Num() - 1);
		NewVert.Color = FColor::White;
	}
}
