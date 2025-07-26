// Fill out your copyright notice in the Description page of Project Settings.

#include "Equipment/EquipmentSlot.h"
#include "Equipment/EquipableItemComponent.h"
#include "Equipment/EquipmentHolderComponent.h"
#include "Equipment.h"

UE_DISABLE_OPTIMIZATION

FEquipmentSlot::FEquipmentSlot()
	: DefaultEquipmentClass(nullptr)
	, SlotId(NAME_None)
	, AttachComponent(FComponentReference())
	, AttachTransform(FTransform::Identity)
	, AttachBone(NAME_None)
	, Equipment(nullptr)
{

}

TSubclassOf<AActor> FEquipmentSlot::GetSlotClass() const
{
	TSubclassOf<AActor> CurrentEquipmentClass = Equipment ? Equipment->GetClass() : nullptr;

	return CurrentEquipmentClass;
}

USceneComponent* FEquipmentSlot::GetAttachComponent(AActor* OwningActor) const
{
	if (USceneComponent* OverrideComponent = Cast<USceneComponent>(AttachComponent.OverrideComponent.Get()))
	{
		return OverrideComponent;
	}

	return Cast<USceneComponent>(AttachComponent.GetComponent(OwningActor));
}

FEquipmentSlot& FEquipmentSlots::CreateSlot(FName SlotId)
{
	FEquipmentSlot& OutSlot = Slots.Emplace_GetRef();
	OutSlot.SlotId = SlotId;
	//MarkSlotDirty(OutSlot);
	return OutSlot;
}

FEquipmentSlot& FEquipmentSlots::GetSlot(FName SlotId)
{
	for (FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return Slot;
		}
	}

	checkNoEntry();
	return Slots[0];
}

const FEquipmentSlot& FEquipmentSlots::GetSlot(FName SlotId) const
{
	for (const FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return Slot;
		}
	}

	checkNoEntry();
	return Slots[0];
}

FEquipmentSlot* FEquipmentSlots::GetSlotMaybeNull(FName SlotId)
{
	for (FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return &Slot;
		}
	}

	return nullptr;
}

const FEquipmentSlot* FEquipmentSlots::GetSlotMaybeNull(FName SlotId) const
{
	for (const FEquipmentSlot& Slot : Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return &Slot;
		}
	}

	return nullptr;
}

UEquipmentHolderComponent* FEquipmentSlots::GetEquipmentHolderComponent() const
{
	check(Cast<UEquipmentHolderComponent>(OwningObject));

	return (UEquipmentHolderComponent*)OwningObject;
}

UE_ENABLE_OPTIMIZATION