// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Misc/Crc.h"
#include "RailConnectionComponent.generated.h"

UENUM(BlueprintType)
enum class ERailConnectionType : uint8
{
	Straight,
	Circular
};

USTRUCT(BlueprintType)
struct FRailConnection
{
	GENERATED_BODY()

	FRailConnection() = default;
	FRailConnection(URailConnectionComponent* InConnection, ERailConnectionType InConnectionType = ERailConnectionType::Straight)
		: Connection(InConnection)
		, ConnectionType(InConnectionType)
	{
	}

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail Connection")
	URailConnectionComponent* Connection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail Connection")
	ERailConnectionType ConnectionType;

	bool operator==(const FRailConnection& Other) const
	{
		return Other.Connection == Connection;
	}
};

static uint32 GetTypeHash(const FRailConnection& Thing)
{
	uint32 Hash = FCrc::MemCrc32(&Thing.Connection, sizeof(Thing.Connection));
	return Hash;
}

UCLASS( ClassGroup=(Rail), meta=(BlueprintSpawnableComponent), Config=Game )
class ROBOTARM_API URailConnectionComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URailConnectionComponent();
		
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	// Connections to other rail connections
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail")
	TSet<FRailConnection> Connections;

	UFUNCTION(BlueprintCallable, Category = "Rail")
	void AddConnection(const FRailConnection& Connection);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rendering")
	bool ShowArrow;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Rail")
	FColor VisualizationColor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail", meta=(ClampMin=1))
	int32 VisualizationSides;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail")
	bool DrawTransformsAlongPath;

	// Draw an arrow pointing to which connection is currently selected
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail")
	bool DrawCurrentConnectionArrow;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Rail")
	FVector CurrentConnectionArrowOffset;

	// Which connection this rail connection is currently connected to.
	// Chooses which path to take when this rail connection is traversed
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail")
	int32 CurrentConnection;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail")
	bool IsStop;

	// Get the positions of each connected component relative to this component
	TArray<FVector> GetConnectionRelativePositions() const;

	// Returns if any of the connections are circular. This is used for drawing the arrow
	bool HasAnyCircularConnections() const;

	// Returns the position along the path, in normalized (0-1) time
	UFUNCTION(BlueprintPure, Category = "Rail")
	FVector GetPosAlongPath(URailConnectionComponent* Connection, float NormalizedTime) const;

	// Returns the position along the path, in normalized (0-1) time
	UFUNCTION(BlueprintPure, Category = "Rail")
	FTransform GetTransformAlongPath(URailConnectionComponent* Connection, float NormalizedTime) const;

	UFUNCTION(BlueprintPure, Category = "Rail")
	bool GetAngleAlongCurve(URailConnectionComponent* Connection, float NormalizedTime, float& OutAngle) const;

	UFUNCTION(BlueprintPure, Category = "Rail")
	bool GetPathLength(URailConnectionComponent* Connection, float& OutLength) const;

	UFUNCTION(BlueprintPure, Category = "Rail")
	URailConnectionComponent* GetCurrentConnection() const;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End UPrimitiveComponent Interface.
};
