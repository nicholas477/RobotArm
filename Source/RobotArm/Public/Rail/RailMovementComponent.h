// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/MovementComponent.h"
#include "RailMovementComponent.generated.h"

class URailConnectionComponent;

/**
 * Follows a rail connection component
 */
UCLASS(ClassGroup = (Rail), meta = (BlueprintSpawnableComponent))
class ROBOTARM_API URailMovementComponent : public UMovementComponent
{
	GENERATED_BODY()
	
public:
	URailMovementComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Rail Movement Component")
	TObjectPtr<URailConnectionComponent> CurrentConnection;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Rail Movement Component", meta=(Units="CentimetersPerSecond"))
	float MovementSpeed;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Rail Movement Component")
	bool bIsMoving;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Rail Movement Component")
	float CurrentLocationOnRail;

	UPROPERTY(SaveGame, BlueprintReadWrite, EditAnywhere, Category = "Rail Movement Component")
	bool bDirectionFlipped;

	UFUNCTION(BlueprintPure, Category = "Rail Movement Component")
	URailConnectionComponent* GetCurrentConnectionEnd() const;

	UFUNCTION(BlueprintPure, Category = "Rail Movement Component")
	float GetCurrentConnectionLength() const;

	UFUNCTION(BlueprintCallable, Category = "Rail Movement Component")
	void FlipDirection();

	UFUNCTION(BlueprintCallable, Category = "Rail Movement Component")
	virtual void UpdateLocationOnRail(bool bTeleport = false);

protected:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
