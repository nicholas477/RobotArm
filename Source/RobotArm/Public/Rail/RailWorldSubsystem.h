// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "RailWorldSubsystem.generated.h"

class URailConnectionComponent;

/**
 * 
 */
UCLASS()
class ROBOTARM_API URailWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Rail")
	TSet<URailConnectionComponent*> Connections;

	// Gets the connection that is in the same location as this connection
	UFUNCTION(BlueprintPure, Category = "Rail")
	URailConnectionComponent* GetAdjacentConnection(URailConnectionComponent* Connection, float DistThreshold = 1.f);
};
