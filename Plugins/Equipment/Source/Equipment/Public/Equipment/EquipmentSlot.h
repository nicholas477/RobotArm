// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquipmentSlot.generated.h"

struct FEquipmentSlots;
class UEquipmentHolderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSlotEquipped, UEquipmentHolderComponent*, EquipmentHolderComponent, AActor*, OldEquipment, AActor*, Equipment);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnSlotEquippedEvent, UEquipmentHolderComponent*, EquipmentHolderComponent, AActor*, OldEquipment, AActor*, Equipment);

USTRUCT(BlueprintType, Blueprintable)
struct EQUIPMENT_API FEquipmentSlot
{
	GENERATED_BODY()

	FEquipmentSlot();

	// The equipment that is spawned by default
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot")
	TSubclassOf<AActor> DefaultEquipmentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", AdvancedDisplay)
	FName SlotId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment Slot", AdvancedDisplay)
	FComponentReference AttachComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", AdvancedDisplay)
	FTransform AttachTransform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slot", AdvancedDisplay)
	FName AttachBone;

	UPROPERTY(BlueprintReadWrite, BlueprintAssignable, Transient, Category = "Equipment Slot")
	FOnSlotEquipped OnSlotEquipped;

	AActor* GetEquipment() const { return Equipment; }
	TSubclassOf<AActor> GetSlotClass() const;
	USceneComponent* GetAttachComponent(AActor* OwningActor = nullptr) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment Slot", AdvancedDisplay)
	TObjectPtr<AActor> Equipment;

	friend UEquipmentHolderComponent;
};

USTRUCT(BlueprintType, Blueprintable)
struct EQUIPMENT_API FEquipmentSlots
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment Slots", meta = (TitleProperty = "{SlotId}:{DefaultEquipmentClass}"))
	TArray<FEquipmentSlot> Slots;

	FEquipmentSlot& CreateSlot(FName SlotId);

	// WARNING: If you edit the returned slot, please mark it dirty!
	FEquipmentSlot& GetSlot(FName SlotId);

	const FEquipmentSlot& GetSlot(FName SlotId) const;

	FEquipmentSlot* GetSlotMaybeNull(FName SlotId);
	const FEquipmentSlot* GetSlotMaybeNull(FName SlotId) const;

	class UEquipmentHolderComponent* GetEquipmentHolderComponent() const;

	UPROPERTY(Transient)
	UObject* OwningObject = nullptr;
};