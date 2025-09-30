// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuildableComponent.generated.h"

class UBuildable;

UCLASS( ClassGroup=(Buildable), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType )
class ROBOTARM_API UBuildableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuildableComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Buildable")
	void SetBuildable(const UBuildable* Buildable);
};
