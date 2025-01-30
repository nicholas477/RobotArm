// Fill out your copyright notice in the Description page of Project Settings.


#include "Rail/RailConnectionComponent.h"

#include "Rail/RailConnectionSceneProxy.h"
#include "Rail/RailWorldSubsystem.h"

UE_DISABLE_OPTIMIZATION
URailConnectionComponent::URailConnectionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;

	bVisibleInReflectionCaptures = false;
	bVisibleInRayTracing = false;
	bVisibleInRealTimeSkyCaptures = false;

	ShowArrow = true;
	VisualizationColor = FColor::White;
	VisualizationSides = 32;
	DrawTransformsAlongPath = false;
	IsStop = false;
}

void URailConnectionComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (UWorld* World = GetWorld())
	{
		if (URailWorldSubsystem* Subsystem = World->GetSubsystem<URailWorldSubsystem>())
		{
			Subsystem->Connections.Add(this);
		}
	}
}

void URailConnectionComponent::UninitializeComponent()
{
	Super::UninitializeComponent();

	if (UWorld* World = GetWorld())
	{
		if (URailWorldSubsystem* Subsystem = World->GetSubsystem<URailWorldSubsystem>())
		{
			Subsystem->Connections.Remove(this);
		}
	}
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

// 2d Ray/Ray intersection
// https://stackoverflow.com/questions/2931573/determining-if-two-rays-intersect
static bool RaysIntersection(const FVector2D& Pos1, const FVector2D& Dir1, const FVector2D& Pos2, const FVector2D& Dir2, FVector2D& Result, float& Dist)
{
	if (Pos1 == Pos2) {
		Result = Pos1;
		return true;
	}
	auto dx = Pos2.X - Pos1.X;
	auto dy = Pos2.Y - Pos1.Y;
	auto det = Dir2.X * Dir1.Y - Dir2.Y * Dir1.X;
	if (det != 0) { // near parallel line will yield noisy results
		double u = (dy * Dir2.X - dx * Dir2.Y) / (double)det;
		double v = (dy * Dir1.X - dx * Dir1.Y) / (double)det;
		if (u >= 0 && v >= 0) {
			Dist = u;
			Result = Pos1 + Dir1 * u;
			return true;
		}
	}
	return false;
}

static FVector2D To2D(const FVector& Vec)
{
	return FVector2D(Vec.X, Vec.Y);
}

FVector URailConnectionComponent::GetPosAlongPath(URailConnectionComponent* Connection, float NormalizedTime) const
{
	if (Connection == nullptr || NormalizedTime <= 0.f || !Connections.Contains(FRailConnection(Connection)))
	{
		return GetComponentTransform().GetLocation();
	}

	if (NormalizedTime >= 1.f)
	{
		return Connection->GetComponentTransform().GetLocation();
	}

	const FRailConnection& RailConnection = *Connections.Find(FRailConnection(Connection));
	
	if (RailConnection.ConnectionType == ERailConnectionType::Straight)
	{
		return FMath::Lerp(GetComponentTransform().GetLocation(), Connection->GetComponentTransform().GetLocation(), NormalizedTime);
	}
	else if (RailConnection.ConnectionType == ERailConnectionType::Circular)
	{
		const FVector2D Pos = To2D(GetComponentTransform().GetLocation());
		const FVector2D OtherPos = To2D(Connection->GetComponentTransform().GetLocation());

		const FVector2D Dir = To2D(GetComponentTransform().TransformVectorNoScale(FVector(1.f, 0.f, 0.f)));
		const FVector2D OtherDir = To2D(Connection->GetComponentTransform().TransformVectorNoScale(FVector(1.f, 0.f, 0.f)));

		float Dist;
		FVector2D Intersect;
		if (RaysIntersection(Pos, Dir, OtherPos, OtherDir, Intersect, Dist))
		{
			// Angle in radians
			const float Ang = FMath::Acos(Dir.Dot(OtherDir));

			const float Sine = 1.f - FMath::Sin(Ang * (1.f - NormalizedTime));
			const float Cos = FMath::Cos(Ang * (1.f - NormalizedTime));

			const float Side = FMath::Sign(GetComponentTransform().InverseTransformPosition(Connection->GetComponentTransform().GetLocation()).Y);

			if (Side > 0.f)
			{
				FVector OutVector = FVector(Cos * Dist, Sine * Dist, 0.f);
				return GetComponentTransform().TransformPosition(OutVector);
			}
			else
			{
				FVector OutVector = FVector(Cos * Dist, Sine * Dist * -1.f, 0.f);
				return GetComponentTransform().TransformPosition(OutVector);
			}
		}
	}

	return FVector(0);
}

FTransform URailConnectionComponent::GetTransformAlongPath(URailConnectionComponent* Connection, float NormalizedTime) const
{
	if (Connection == nullptr || NormalizedTime <= 0.f || !Connections.Contains(FRailConnection(Connection)))
	{
		return GetComponentTransform();
	}

	const FRailConnection& RailConnection = *Connections.Find(FRailConnection(Connection));
	if (RailConnection.ConnectionType == ERailConnectionType::Straight)
	{
		FTransform OutTransform = GetComponentTransform();
		OutTransform.SetLocation(GetPosAlongPath(Connection, NormalizedTime));
		return OutTransform;
	}
	else if (RailConnection.ConnectionType == ERailConnectionType::Circular)
	{
		float Angle;
		if (GetAngleAlongCurve(Connection, NormalizedTime, Angle))
		{
			const float Side = FMath::Sign(GetComponentTransform().InverseTransformPosition(Connection->GetComponentTransform().GetLocation()).Y);

			FVector XVector = GetComponentTransform().TransformVectorNoScale(FVector(FMath::Cos(Angle), Side * FMath::Sin(Angle), 0.f));
			FVector YVector = GetComponentTransform().TransformVectorNoScale(FVector(-Side * FMath::Sin(Angle), FMath::Cos(Angle), 0.f));

			FMatrix Mat = FRotationMatrix::MakeFromXY(XVector, YVector);
			Mat.SetOrigin(GetPosAlongPath(Connection, NormalizedTime));
			return FTransform(Mat);
		}
	}

	return GetComponentTransform();
}

bool URailConnectionComponent::GetAngleAlongCurve(URailConnectionComponent* Connection, float NormalizedTime, float& OutAngle) const
{
	if (Connection == nullptr)
	{
		return false;
	}

	if (NormalizedTime <= 0.f)
	{
		OutAngle = 0.f;
		return true;
	}

	NormalizedTime = FMath::Clamp(NormalizedTime, 0.f, 1.f);

	const FVector2D Dir = To2D(GetComponentTransform().TransformVectorNoScale(FVector(1.f, 0.f, 0.f)));
	const FVector2D OtherDir = To2D(Connection->GetComponentTransform().TransformVectorNoScale(FVector(1.f, 0.f, 0.f)));

	// Angle in radians
	OutAngle = FMath::Acos(Dir.Dot(OtherDir)) * NormalizedTime;
	return true;
}

bool URailConnectionComponent::GetPathLength(URailConnectionComponent* Connection, float& OutLength) const
{
	if (Connection == nullptr)
	{
		return false;
	}

	const FRailConnection* RailConnection = Connections.Find(FRailConnection(Connection));
	if (RailConnection == nullptr)
	{
		return false;
	}

	if (RailConnection->ConnectionType == ERailConnectionType::Straight)
	{
		OutLength = FVector::Distance(GetComponentLocation(), Connection->GetComponentLocation());
		return true;
	}
	else if (RailConnection->ConnectionType == ERailConnectionType::Circular)
	{
		const FVector2D Pos = To2D(GetComponentTransform().GetLocation());
		const FVector2D OtherPos = To2D(Connection->GetComponentTransform().GetLocation());

		const FVector2D Dir = To2D(GetComponentTransform().TransformVectorNoScale(FVector(1.f, 0.f, 0.f)));
		const FVector2D OtherDir = To2D(Connection->GetComponentTransform().TransformVectorNoScale(FVector(1.f, 0.f, 0.f)));

		float Dist;
		FVector2D Intersect;
		if (RaysIntersection(Pos, Dir, OtherPos, OtherDir, Intersect, Dist))
		{
			float Angle;
			if (GetAngleAlongCurve(Connection, 1.f, Angle))
			{
				OutLength = Angle * Dist;
				return true;
			}
		}
	}

	return false;
}
UE_ENABLE_OPTIMIZATION
