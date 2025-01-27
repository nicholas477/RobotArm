// Fill out your copyright notice in the Description page of Project Settings.


#include "Rail/RailConnectionComponent.h"

#include "Rail/RailConnectionSceneProxy.h"

URailConnectionComponent::URailConnectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bVisibleInReflectionCaptures = false;
	bVisibleInRayTracing = false;
	bVisibleInRealTimeSkyCaptures = false;

	VisualizationColor = FColor::White;
}

void URailConnectionComponent::AddConnection(const FRailConnection& Connection)
{
	if (Connection.Connection != nullptr)
	{
		if (!Connections.Contains(Connection))
		{
			Connections.Add(Connection);
			MarkRenderStateDirty();
		}

		if (!Connection.Connection->Connections.Contains(FRailConnection(this)))
		{
			FRailConnection NewConnection = Connection;
			NewConnection.Connection = this;
			Connection.Connection->AddConnection(NewConnection);
		}
	}
}

FPrimitiveSceneProxy* URailConnectionComponent::CreateSceneProxy()
{
	return new FRailConnectionSceneProxy(this);
}

FBoxSphereBounds URailConnectionComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	TArray<FVector> ConnectionsArray = GetConnectionRelativePositions();
	ConnectionsArray.Add(FVector(0.f, 0.f, 0.f));

	for (FVector& Connection : ConnectionsArray)
	{
		Connection = LocalToWorld.TransformPosition(Connection);
	}

	return FBoxSphereBounds(ConnectionsArray.GetData(), ConnectionsArray.Num());
}

TArray<FVector> URailConnectionComponent::GetConnectionRelativePositions() const
{
	TArray<FVector> Positions;
		 
	for (const FRailConnection& Connection : Connections)
	{
		if (Connection != nullptr)
		{
			FVector ConnectionPos = Connection.Connection->GetComponentLocation();

			ConnectionPos = GetComponentTransform().InverseTransformPosition(ConnectionPos);
			Positions.Add(ConnectionPos);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Num connections: %d"), Positions.Num());

	return Positions;
}

bool URailConnectionComponent::HasAnyCircularConnections() const
{
	for (const FRailConnection& Connection : Connections)
	{
		if (Connection != nullptr)
		{
			if (Connection.ConnectionType == ERailConnectionType::Circular)
			{
				return true;
			}
		}
	}

	return false;
}
