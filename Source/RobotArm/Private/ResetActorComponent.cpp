// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetActorComponent.h"

#include "ResetActorWorldSubsystem.h"

// Sets default values for this component's properties
UResetActorComponent::UResetActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	bWantsInitializeComponent = true;

	ResetActorTransform = true;
}

void UResetActorComponent::InitializeComponent()
{
	Super::InitializeComponent();

	ActorTransform = GetOwner()->GetTransform();

	if (GetWorld())
	{
		if (UResetActorWorldSubsystem* ResetActorSubsystem = GetWorld()->GetSubsystem<UResetActorWorldSubsystem>())
		{
			ResetActorSubsystem->AddResetActorComponent(this);
		}
	}
}

void UResetActorComponent::UninitializeComponent()
{
	if (GetWorld())
	{
		if (UResetActorWorldSubsystem* ResetActorSubsystem = GetWorld()->GetSubsystem<UResetActorWorldSubsystem>())
		{
			ResetActorSubsystem->RemoveResetActorComponent(this);
		}
	}

	Super::UninitializeComponent();
}

void UResetActorComponent::ResetActor_Implementation()
{
	if (ResetActorTransform)
	{
		GetOwner()->SetActorTransform(ActorTransform, false, nullptr, ETeleportType::ResetPhysics);
	}

	OnActorReset.Broadcast(GetOwner(), this, ActorTransform);
}

