// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Buildable.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTARM_API UBuildable : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Name for this buildable
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Buildable")
	FText Name;

	// Class for the spawned buildable
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Buildable")
	TSoftClassPtr<AActor> Class;

	// Class for the buildable preview
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Buildable")
	TSoftClassPtr<AActor> PreviewClass;

	// UI thumbnail
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Buildable")
	TSoftObjectPtr<UTexture2D> Image;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Buildable")
	int32 Cost;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Buildable")
	FGameplayTagContainer Tags;
};
