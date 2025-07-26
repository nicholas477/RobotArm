// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipableItemComponent.h"
#include "Equipment/EquipmentHolderComponent.h"
#include "Equipment.h"

UEquipableItemComponent::UEquipableItemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bIsEquipped = false;
	EquipmentHolderComponent = nullptr;
}

void UEquipableItemComponent::OnEquip(AActor* LastItem, bool bFromSave)
{
	if (!bIsEquipped && ensure(EquipmentHolderComponent.IsValid()))
	{
		bIsEquipped = true;

		if (GetOwner() && EquipmentHolderComponent->GetOwner())
		{
			GetOwner()->SetInstigator(EquipmentHolderComponent->GetOwner()->GetInstigator());
		}

		BP_OnEquip(LastItem, bFromSave);
		OnEquippedDelegate.Broadcast(this, LastItem, bFromSave);
	}
}

void UEquipableItemComponent::OnUnEquip(bool bFromSave)
{
	if (bIsEquipped)
	{
		bIsEquipped = false;

		if (GetOwner())
		{
			GetOwner()->SetInstigator(nullptr);
		}

		BP_OnUnEquip(bFromSave);
		OnUnEquippedDelegate.Broadcast(this, bFromSave);
	}
}

FText UEquipableItemComponent::GetEquipmentName_Implementation()
{
	if (GetEquipmentNameDelegate.IsBound())
	{
		return GetEquipmentNameDelegate.Execute(this);
	}

	return EquipmentName;
}

bool UEquipableItemComponent::CanEquip_Implementation(const UEquipmentHolderComponent* InEquipmentHolderComponent) const
{
	return true;
}

UEquipmentHolderComponent* UEquipableItemComponent::GetEquipmentHolderComponent() const
{
	return EquipmentHolderComponent.Get();
}

bool UEquipableItemComponent::GetSlot(FEquipmentSlot& Slot) const
{
	if (UEquipmentHolderComponent* Holder = GetEquipmentHolderComponent())
	{
		return Holder->GetSlotFromEquipment(GetOwner(), Slot);
	}
	return false;
}

APawn* UEquipableItemComponent::GetHoldingPawn() const
{
	if (UEquipmentHolderComponent* Holder = GetEquipmentHolderComponent())
	{
		return Cast<APawn>(Holder->GetOwner());
	}
	return nullptr;
}

void UEquipableItemComponent::OnRegister()
{
	Super::OnRegister();

#if WITH_EDITOR
	AsyncTask(ENamedThreads::GameThread, [WeakThis = TWeakObjectPtr(this)]()
	{
		if (!WeakThis.IsValid())
		{
			return;
		}

		if (WeakThis->EquipmentHolderComponent.IsValid())
		{
			if (!WeakThis->EquipmentHolderComponent->HasEquipment(WeakThis->GetOwner()))
			{
				ensure(false);
				UE_LOG(LogEquipment, Warning, TEXT("Equipment has no holder component! %s"), *WeakThis->GetOwner()->GetName());

				WeakThis->GetOwner()->Destroy();
			}
		}
	});
#endif
}

void UEquipableItemComponent::OnUnregister()
{
	if (!IsUnreachable())
	{
		OnUnEquip();
	}

	Super::OnUnregister();
}
