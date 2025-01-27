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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROBOTARM_API URailConnectionComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URailConnectionComponent();
		
	// Connections to other rail connections
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail")
	TSet<FRailConnection> Connections;

	UFUNCTION(BlueprintCallable, Category = "Rail")
	void AddConnection(const FRailConnection& Connection);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Rail")
	FColor VisualizationColor;

	// Get the positions of each connected component relative to this component
	TArray<FVector> GetConnectionRelativePositions() const;

	// Returns if any of the connections are circular. This is used for drawing the arrow
	bool HasAnyCircularConnections() const;

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	//~ End UPrimitiveComponent Interface.
};
