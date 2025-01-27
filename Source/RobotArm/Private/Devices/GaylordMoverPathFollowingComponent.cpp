// Fill out your copyright notice in the Description page of Project Settings.


#include "Devices/GaylordMoverPathFollowingComponent.h"

#include "AIController.h"

void UGaylordMoverPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
	if (!Path.IsValid() || NavMovementInterface == nullptr)
	{
		return;
	}

	const FVector CurrentLocation = NavMovementInterface->GetFeetLocation();
	const FVector CurrentAimingDirection = NavMovementInterface->GetForwardVector();
	const FVector CurrentTarget = GetCurrentTargetLocation();

	//DrawDebugSphere(GetWorld(), CurrentTarget, 32.f, 32, FColor::Red, false, 0.f);

	FVector TargetDirection = CurrentTarget - CurrentLocation;
	TargetDirection.Z = 0.f;
	TargetDirection.Normalize();

	const float AngDifference = FMath::RadiansToDegrees(FMath::Acos(TargetDirection.CosineAngle2D(CurrentAimingDirection)));

	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, "UGaylordMoverPathFollowingComponent::FollowPathSegment(float DeltaTime)");
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Red, FString::Printf(L"Angle difference: %f", AngDifference));

	//if (AngDifference <= 1.f)
	{
		Super::FollowPathSegment(DeltaTime);
	}
	//else
	//{
	//	if (AAIController* Controller = Cast<AAIController>(GetOwner()))
	//	{
	//		Controller->SetControlRotation(CurrentTarget.ToOrientationRotator());
	//	}
	//}
}
