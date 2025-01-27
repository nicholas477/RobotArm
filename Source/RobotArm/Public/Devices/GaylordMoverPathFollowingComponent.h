// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "GaylordMoverPathFollowingComponent.generated.h"

/**
 * 
 */
UCLASS()
class ROBOTARM_API UGaylordMoverPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()
	
protected:
	virtual void FollowPathSegment(float DeltaTime) override;
};
