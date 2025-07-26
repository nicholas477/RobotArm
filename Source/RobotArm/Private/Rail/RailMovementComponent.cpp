// Fill out your copyright notice in the Description page of Project Settings.


#include "Rail/RailMovementComponent.h"

#include "Rail/RailConnectionComponent.h"
#include "Rail/RailWorldSubsystem.h"

URailMovementComponent::URailMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	CurrentConnection = nullptr;
	bIsMoving = false;
	CurrentLocationOnRail = 0.f;
	MovementSpeed = 50.f;
}

URailConnectionComponent* URailMovementComponent::GetCurrentConnectionEnd() const
{
	if (CurrentConnection != nullptr)
	{
		return CurrentConnection->GetCurrentConnection();
	}
	return nullptr;
}

float URailMovementComponent::GetCurrentConnectionLength() const
{
	if (CurrentConnection != nullptr)
	{
		float OutLength;
		if (CurrentConnection->GetPathLength(GetCurrentConnectionEnd(), OutLength))
		{
			return OutLength;
		}
	}
	return 0.0f;
}

void URailMovementComponent::FlipDirection()
{
	CurrentConnection = GetCurrentConnectionEnd();
	CurrentLocationOnRail = 1.f - CurrentLocationOnRail;
	bDirectionFlipped = !bDirectionFlipped;
}

void URailMovementComponent::UpdateLocationOnRail(bool bTeleport)
{
	if (UpdatedComponent != nullptr && CurrentConnection != nullptr)
	{
		const FTransform PathTransform = CurrentConnection->GetTransformAlongPath(GetCurrentConnectionEnd(), CurrentLocationOnRail);
		
		FTransform RelativeTransform = FTransform::Identity;
		if (bDirectionFlipped)
		{
			RelativeTransform.SetRotation(FRotator(0.f, 0.f, 180.f).Quaternion());
		}

		const FTransform FinalTransform = RelativeTransform * PathTransform;

		const FVector Delta = FinalTransform.GetLocation() - UpdatedComponent->GetComponentLocation();


		FHitResult Hit(1.f);
		SafeMoveUpdatedComponent(Delta, FinalTransform.GetRotation(), !bTeleport, Hit, bTeleport ? ETeleportType::TeleportPhysics : ETeleportType::None);

		if (Hit.IsValidBlockingHit())
		{
			HandleImpact(Hit, GetWorld()->GetDeltaSeconds(), Delta);
			// Try to slide the remaining distance along the surface.
			//SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);
		}
	}
}

void URailMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentConnection != nullptr)
	{
		if (bIsMoving)
		{
			CurrentLocationOnRail += (DeltaTime * MovementSpeed) / GetCurrentConnectionLength();
			while (CurrentLocationOnRail > 1.f && CurrentConnection != nullptr)
			{
				CurrentLocationOnRail = FMath::Frac(CurrentLocationOnRail) * GetCurrentConnectionLength();
				CurrentConnection = GetWorld()->GetSubsystem<URailWorldSubsystem>()->GetAdjacentConnection(GetCurrentConnectionEnd());
				CurrentLocationOnRail = CurrentLocationOnRail / GetCurrentConnectionLength();
			}
			UpdateLocationOnRail(true);
		}
	}
}
