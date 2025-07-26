// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/Class.h"
#include "Equipment/EquipmentSlot.h"
#include "EquipmentHolderComponent.generated.h"

UCLASS( ClassGroup=(Equipment), meta=(BlueprintSpawnableComponent) )
class EQUIPMENT_API UEquipmentHolderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentHolderComponent(const FObjectInitializer& ObjectInitializer);
	virtual void InitializeComponent() override;
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void BeginDestroy() override;

	FEquipmentSlot& GetOrCreateSlotById(FName SlotId);

	const FEquipmentSlots& GetEquipmentSlots() const { return EquipmentSlots; }

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		FEquipmentSlots& GetEquipmentSlots() { return EquipmentSlots; }

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		bool GetEquipmentSlot(FName SlotID, FEquipmentSlot& Slot);

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		bool BindToOnSlotEquipped(FName SlotID, FOnSlotEquippedEvent Delegate);

	// Gets the component that the equipment attaches to at this slot
	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		USceneComponent* GetEquipmentSlotAttachComponent(FName SlotID) const;

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		void SetEquipmentSlot(const FEquipmentSlot& Slot);

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		bool SetSlotEquipment(FName SlotID, TSubclassOf<AActor> Class, bool bDestroyOldEquipment = true, bool bUpdateDefaultEquipmentClass = true);

	// Returns true if the slot was found and set
	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		bool SetEquipmentSlotAttachComponent(FName SlotID, USceneComponent* AttachComponent);

	void SetSlotEquipment(FEquipmentSlot& Slot, AActor* NewEquipment, bool bDestroyOldEquipment = true, bool bUpdateDefaultEquipmentClass = true);
	void SetSlotEquipment(FEquipmentSlot& Slot, TSubclassOf<AActor> NewEquipmentClass, bool bDestroyOldEquipment = true, bool bUpdateDefaultEquipmentClass = true);

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		void RespawnSlotActor(FName SlotID);

	UFUNCTION(BlueprintCallable, Category = "Equipment Holder")
		bool UnequipActor(AActor* Actor, bool bDestroyActor = false);

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		bool HasEquipment(AActor* Equipment) const;

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		bool GetSlotFromEquipment(const AActor* Equipment, FEquipmentSlot& Slot);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Equipment Holder")
	virtual void RefreshEquipment();

	UPROPERTY(EditDefaultsOnly, Category = "Equipment Holder")
		bool bEquipmentActorsAreTransient;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_EquipmentSlots, Category = "Equipment Holder")
		FEquipmentSlots EquipmentSlots;

	UFUNCTION()
		virtual void OnRep_EquipmentSlots(FEquipmentSlots& OldEquipmentSlots);

	void PerformEquipmentSlotDeltas(FEquipmentSlot* OldSlot, FEquipmentSlot* NewSlot);
	
	// Calls all of the associated equip/unequip functions on an actor. Does not change slots
	void PerformUnequip(FEquipmentSlot& Slot, AActor* OldEquipment, bool bDestroyOldEquipment = true);
	void PerformEquip(FEquipmentSlot& Slot, AActor* OldEquipment, AActor* NewEquipment);

	UFUNCTION()
		void OnChildEquipmentDestroyed(AActor* DestroyedActor);

	UFUNCTION(BlueprintPure, Category = "Equipment Holder")
		virtual TSubclassOf<AActor> GetDefaultEquipmentClass(const FEquipmentSlot& Slot);

	// Just spawns and attaches the actor of EquipmentClass to the slot, does not call OnEquip!
	AActor* SpawnEquipmentClass(const FEquipmentSlot& Slot, TSubclassOf<AActor> EquipmentClass);

	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostEditImport() override;
};
