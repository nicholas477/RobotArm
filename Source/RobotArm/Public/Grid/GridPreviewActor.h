// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridPreviewActor.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ROBOTARM_API AGridPreviewActor : public AActor
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	void SetEditable(bool bIsEditable) { bEditable = bIsEditable; }
#endif
};
