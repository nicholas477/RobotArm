// Fill out your copyright notice in the Description page of Project Settings.


#include "Buildable/BuildableComponent.h"

UBuildableComponent::UBuildableComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UBuildableComponent::SetBuildable_Implementation(const UBuildable* Buildable)
{
}
