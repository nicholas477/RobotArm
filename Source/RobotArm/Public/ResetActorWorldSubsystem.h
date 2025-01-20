// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ResetActorWorldSubsystem.generated.h"

class UResetActorComponent;

/**
 * 
 */
UCLASS()
class ROBOTARM_API UResetActorWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Reset Actor Subsystem")
	TSet<UResetActorComponent*> ResetActorComponents;

	UFUNCTION(BlueprintCallable, Category = "Reset Actor Subsystem")
	void ResetActors();

	void AddResetActorComponent(UResetActorComponent* Component);
	bool RemoveResetActorComponent(UResetActorComponent* Component);
};
