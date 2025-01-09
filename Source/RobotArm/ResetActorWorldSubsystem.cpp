// Fill out your copyright notice in the Description page of Project Settings.


#include "ResetActorWorldSubsystem.h"

#include "ResetActorComponent.h"

void UResetActorWorldSubsystem::ResetActors()
{
	for (UResetActorComponent* Component : ResetActorComponents)
	{
		if (IsValid(Component))
		{
			Component->ResetActor();
		}
	}
}

void UResetActorWorldSubsystem::AddResetActorComponent(UResetActorComponent* Component)
{
	if (IsValid(Component))
	{
		ResetActorComponents.Add(Component);
	}
}

bool UResetActorWorldSubsystem::RemoveResetActorComponent(UResetActorComponent* Component)
{
	return ResetActorComponents.Remove(Component) > 0;
}
