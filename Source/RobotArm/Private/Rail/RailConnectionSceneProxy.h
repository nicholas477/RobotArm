// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Materials/Material.h"
#include "Materials/MaterialRenderProxy.h"
#include "DynamicMeshBuilder.h"

class URailConnectionComponent;

class FRailConnectionSceneProxy : public FPrimitiveSceneProxy
{
public:
	FRailConnectionSceneProxy(const URailConnectionComponent* InComponent);

	virtual ~FRailConnectionSceneProxy();

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override;

	/**
	*  Returns a struct that describes to the renderer when to draw this proxy.
	*	@param		Scene view to use to determine our relevence.
	*  @return		View relevance struct
	*/
	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override;

	virtual uint32 GetMemoryFootprint(void) const override;

	uint32 GetAllocatedSize(void) const;

	SIZE_T GetTypeHash() const override;

private:
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 LineIndexBuffer;
	FLocalVertexFactory VertexFactory;

	const URailConnectionComponent* RailConnectionComponent;

	void CreateLineBatch(int32 ViewIndex, FMeshElementCollector& Collector) const;
	void DrawArrow(int32 ViewIndex, FMeshElementCollector& Collector, const FSceneView* View) const;

	FColor Color;
	bool bDrawArrow;

	void CreateLineData(TArray<FDynamicMeshVertex>& OutVerts);
};
