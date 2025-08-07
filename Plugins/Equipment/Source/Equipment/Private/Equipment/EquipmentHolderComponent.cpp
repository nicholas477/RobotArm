// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment/EquipmentHolderComponent.h"

#include "Equipment/EquipableItemComponent.h"
#include "Net/UnrealNetwork.h"

UE_DISABLE_OPTIMIZATION
static bool IsClassReplicated(TSubclassOf<AActor> Class)
{
	AActor* ChildClassCDO = (Class ? Class->GetDefaultObject<AActor>() : nullptr);
	const bool bChildActorClassReplicated = ChildClassCDO && ChildClassCDO->GetIsReplicated();
	return bChildActorClassReplicated;
}

UEquipmentHolderComponent::UEquipmentHolderComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	//bAllowReregistration = false;
	SetIsReplicatedByDefault(true);

	bEquipmentActorsAreTransient = false;
}

void UEquipmentHolderComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//return;

	check(!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject));

	EquipmentSlots.OwningObject = this;

	//RefreshEquipment();
}

void UEquipmentHolderComponent::OnRegister()
{
	Super::OnRegister();

	//return;

	check(!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject));

	// Call refresh equipment one frame after registration
	AsyncTask(ENamedThreads::GameThread, [WeakThis = TWeakObjectPtr<ThisClass>(this)]() {
		if (WeakThis.IsValid())
		{
			WeakThis->EquipmentSlots.OwningObject = WeakThis.Get();
			WeakThis->RefreshEquipment();
		}
	});
}

void UEquipmentHolderComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UEquipmentHolderComponent::BeginDestroy()
{
	//for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	//{
	//	SetSlotEquipment(Slot, nullptr, true, false);
	//}

	Super::BeginDestroy();
}

void UEquipmentHolderComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UEquipmentHolderComponent, EquipmentSlots);
}

FEquipmentSlot& UEquipmentHolderComponent::GetOrCreateSlotById(FName SlotId)
{
	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.SlotId == SlotId)
		{
			return Slot;
		}
	}

	return EquipmentSlots.CreateSlot(SlotId);
}

bool UEquipmentHolderComponent::GetEquipmentSlot(FName SlotID, FEquipmentSlot& Slot)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == SlotID)
		{
			Slot = EquipSlot;
			return true;
		}
	}

	return false;
}

bool UEquipmentHolderComponent::BindToOnSlotEquipped(FName SlotID, FOnSlotEquippedEvent Delegate)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == SlotID)
		{
			EquipSlot.OnSlotEquipped.AddUnique(Delegate);
			return true;
		}
	}

	return false;
}

USceneComponent* UEquipmentHolderComponent::GetEquipmentSlotAttachComponent(FName SlotID) const
{
	FEquipmentSlot Slot;
	if (const_cast<UEquipmentHolderComponent*>(this)->GetEquipmentSlot(SlotID, Slot))
	{
		return Slot.GetAttachComponent(GetOwner());
	}
	return nullptr;
}

void UEquipmentHolderComponent::SetEquipmentSlot(const FEquipmentSlot& Slot)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == Slot.SlotId)
		{
			EquipSlot = Slot;
			return;
		}
	}

	EquipmentSlots.Slots.Add(Slot);
}

bool UEquipmentHolderComponent::SetSlotEquipment(FName SlotID, TSubclassOf<AActor> Class, bool bDestroyOldEquipment, bool bUpdateDefaultEquipmentClass)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == SlotID)
		{
			SetSlotEquipment(EquipSlot, Class, bDestroyOldEquipment, bUpdateDefaultEquipmentClass);
			return true;
		}
	}

	return false;
}

bool UEquipmentHolderComponent::SetEquipmentSlotAttachComponent(FName SlotID, USceneComponent* AttachComponent)
{
	for (FEquipmentSlot& EquipSlot : EquipmentSlots.Slots)
	{
		if (EquipSlot.SlotId == SlotID)
		{
			EquipSlot.AttachComponent.OverrideComponent = AttachComponent;
			if (EquipSlot.Equipment && AttachComponent)
			{
				EquipSlot.Equipment->AttachToComponent(AttachComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, EquipSlot.AttachBone);
			}
			return true;
		}
	}

	return false;
}

void UEquipmentHolderComponent::SetSlotEquipment(FEquipmentSlot& Slot, AActor* NewEquipment, bool bDestroyOldEquipment, bool bUpdateDefaultEquipmentClass)
{
	const TSubclassOf<AActor> CurrentEquipmentClass = Slot.GetSlotClass();
	const TSubclassOf<AActor> NewEquipmentClass = NewEquipment ? NewEquipment->GetClass() : nullptr;

	if (NewEquipment != Slot.Equipment || CurrentEquipmentClass != NewEquipmentClass)
	{
		AActor* OldEquipment = Slot.Equipment;
		if (bUpdateDefaultEquipmentClass && (HasBegunPlay() || GetOwner()->IsRunningUserConstructionScript()))
		{
			Slot.DefaultEquipmentClass = NewEquipment ? NewEquipment->GetClass() : nullptr;
		}
		Slot.Equipment = NewEquipment;

		if (OldEquipment)
		{
			PerformUnequip(Slot, OldEquipment, bDestroyOldEquipment);
		}

		if (NewEquipment)
		{
			PerformEquip(Slot, OldEquipment, NewEquipment);
		}
	}
}

void UEquipmentHolderComponent::SetSlotEquipment(FEquipmentSlot& Slot, TSubclassOf<AActor> NewEquipmentClass, bool bDestroyOldEquipment, bool bUpdateDefaultEquipmentClass)
{
	const TSubclassOf<AActor> CurrentEquipmentClass = Slot.GetSlotClass();

	if (NewEquipmentClass != CurrentEquipmentClass)
	{
		AActor* NewEquipment = SpawnEquipmentClass(Slot, NewEquipmentClass);

		SetSlotEquipment(Slot, NewEquipment, bDestroyOldEquipment, bUpdateDefaultEquipmentClass);
	}
}

void UEquipmentHolderComponent::RespawnSlotActor(FName SlotID)
{
	//return;

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.SlotId == SlotID && GetDefaultEquipmentClass(Slot))
		{
			SetSlotEquipment(Slot, GetDefaultEquipmentClass(Slot));
			return;
		}
	}
}

bool UEquipmentHolderComponent::UnequipActor(AActor* Actor, bool bDestroyActor)
{
	if (Actor == nullptr)
		return false;

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment == Actor)
		{
			SetSlotEquipment(Slot, (AActor*)nullptr, bDestroyActor);
			return true;
		}
	}

	return false;
}

bool UEquipmentHolderComponent::HasEquipment(AActor* Equipment) const
{
	for (const FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment == Equipment)
		{
			return true;
		}
	}

	return false;
}

bool UEquipmentHolderComponent::GetSlotFromEquipment(const AActor* Equipment, FEquipmentSlot& InSlot)
{
	for (const FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment == Equipment)
		{
			InSlot = Slot;
			return true;
		}
	}

	return false;
}

void UEquipmentHolderComponent::OnRep_EquipmentSlots(FEquipmentSlots& OldEquipmentSlots)
{
	TArray<FName> SlotIds;

	for (const FEquipmentSlot& Slot : OldEquipmentSlots.Slots)
	{
		SlotIds.AddUnique(Slot.SlotId);
	}

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		SlotIds.AddUnique(Slot.SlotId);
	}

	for (FName SlotId : SlotIds)
	{
		FEquipmentSlot* OldSlot = OldEquipmentSlots.GetSlotMaybeNull(SlotId);
		FEquipmentSlot* NewSlot = EquipmentSlots.GetSlotMaybeNull(SlotId);

		PerformEquipmentSlotDeltas(OldSlot, NewSlot);
	}
}

void UEquipmentHolderComponent::PerformEquipmentSlotDeltas(FEquipmentSlot* OldSlot, FEquipmentSlot* NewSlot)
{
	//return;

	// If the new slot has a non-replicated actor class then we need to spawn it before doing the deltas
	if (NewSlot && NewSlot->DefaultEquipmentClass != nullptr && !IsClassReplicated(NewSlot->DefaultEquipmentClass) && NewSlot->Equipment == nullptr)
	{
		if (OldSlot && OldSlot->Equipment && OldSlot->Equipment->GetClass() == NewSlot->DefaultEquipmentClass)
		{
			// We can carry over the old slot equipment if the class hasn't changed
			NewSlot->Equipment = OldSlot->Equipment;
		}

		// If we didn't carry over the old slot equipment then we gotta spawn it client side
		if (NewSlot->Equipment == nullptr)
		{
			NewSlot->Equipment = SpawnEquipmentClass(*NewSlot, NewSlot->DefaultEquipmentClass);
		}
	}

	AActor* LastActor = OldSlot ? OldSlot->Equipment : nullptr;
	AActor* NewActor = NewSlot ? NewSlot->Equipment : nullptr;

	if (LastActor == NewActor)
	{
		return;
	}
	else
	{
		if (LastActor)
		{
			PerformUnequip(*OldSlot, LastActor, true);
		}

		if (NewActor)
		{
			PerformEquip(*NewSlot, LastActor, NewActor);
		}
	}
}

void UEquipmentHolderComponent::PerformUnequip(FEquipmentSlot& Slot, AActor* OldEquipment, bool bDestroyOldEquipment)
{
	if (OldEquipment)
	{
		Slot.OnSlotEquipped.Broadcast(this, OldEquipment, nullptr);

		const bool bIsEquipmentPendingKillOrUnreachable = !IsValidChecked(OldEquipment) || OldEquipment->IsUnreachable();
		if (!GExitPurge && !bIsEquipmentPendingKillOrUnreachable)
		{
			if (UEquipableItemComponent* EIC = Cast<UEquipableItemComponent>(OldEquipment->GetComponentByClass(UEquipableItemComponent::StaticClass())))
			{
				if (EIC->EquipmentHolderComponent == this)
				{
					EIC->OnUnEquip();
					EIC->EquipmentHolderComponent = nullptr;
				}
			}

			if (bDestroyOldEquipment)
			{
				// Don't destroy this actor if its replicated and we don't have auth
				if (!GetOwner()->HasAuthority() && OldEquipment->GetIsReplicated())
				{
					return;
				}

				OldEquipment->Destroy();
				if (Slot.Equipment == OldEquipment)
				{
					Slot.Equipment = nullptr;
				}
			}
		}
	}
}

void UEquipmentHolderComponent::PerformEquip(FEquipmentSlot& Slot, AActor* OldEquipment, AActor* NewEquipment)
{
	if (NewEquipment == OldEquipment)
	{
		return;
	}

	if (NewEquipment)
	{
		NewEquipment->SetOwner(GetOwner());
		NewEquipment->SetInstigator(GetOwner()->GetInstigator());
		if (Slot.GetAttachComponent(GetOwner()))
		{
			NewEquipment->AttachToComponent(Slot.GetAttachComponent(GetOwner()), FAttachmentTransformRules::SnapToTargetIncludingScale, Slot.AttachBone);
			NewEquipment->SetActorRelativeTransform(Slot.AttachTransform);
		}

		NewEquipment->OnDestroyed.RemoveAll(this);
		NewEquipment->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnChildEquipmentDestroyed);

		if (UEquipableItemComponent* EIC = Cast<UEquipableItemComponent>(NewEquipment->GetComponentByClass(UEquipableItemComponent::StaticClass())))
		{
			EIC->EquipmentHolderComponent = this;
			EIC->OnEquip(OldEquipment);
		}

		Slot.OnSlotEquipped.Broadcast(this, OldEquipment, NewEquipment);
	}
}

void UEquipmentHolderComponent::OnChildEquipmentDestroyed(AActor* DestroyedActor)
{
	if (GExitPurge)
	{
		return;
	}

	//for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	//{
	//	if (Slot.Equipment == DestroyedActor)
	//	{
	//		SetSlotEquipment(Slot, (AActor*)nullptr, false);
	//		return;
	//	}
	//}
}

void UEquipmentHolderComponent::RefreshEquipment()
{
	EquipmentSlots.OwningObject = this;

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		SetSlotEquipment(Slot, GetDefaultEquipmentClass(Slot), true, false);
		PerformEquip(Slot, nullptr, Slot.Equipment);
	}
}

TSubclassOf<AActor> UEquipmentHolderComponent::GetDefaultEquipmentClass(const FEquipmentSlot& Slot)
{
	return Slot.DefaultEquipmentClass;
}

AActor* UEquipmentHolderComponent::SpawnEquipmentClass(const FEquipmentSlot& Slot, TSubclassOf<AActor> EquipmentClass)
{
	if (EquipmentClass == nullptr)
	{
		return nullptr;
	}

	if (GetOwner() == nullptr)
	{
		return nullptr;
	}

	const bool ShouldSpawnEquipment = GetOwner()->HasAuthority() || !IsClassReplicated(EquipmentClass);
	if (ShouldSpawnEquipment && GetWorld())
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnInfo.bAllowDuringConstructionScript = true;
		if (GetWorld()->WorldType == EWorldType::EditorPreview)
		{
			SpawnInfo.ObjectFlags |= EObjectFlags::RF_Transient;
		}
		SpawnInfo.Owner = GetOwner();

		FTransform SpawnTransform = GetOwner() ? GetOwner()->GetTransform() : FTransform::Identity;
		if (Slot.GetAttachComponent(GetOwner()))
		{
			SpawnTransform.SetLocation(Slot.GetAttachComponent(GetOwner())->GetSocketLocation(Slot.AttachBone));
			SpawnTransform.SetRotation(Slot.GetAttachComponent(GetOwner())->GetSocketRotation(Slot.AttachBone).Quaternion());
		}
		AActor* NewEquipment = GetWorld()->SpawnActor(EquipmentClass, &SpawnTransform, SpawnInfo);

		if (GetOwner()->GetLocalRole() == ENetRole::ROLE_Authority)
		{
			NewEquipment->SetAutonomousProxy(true);
		}

		return NewEquipment;
	}
	return nullptr;
}

#if WITH_EDITOR
void UEquipmentHolderComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

void UEquipmentHolderComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UEquipmentHolderComponent::PostEditImport()
{
	Super::PostEditImport();

	//return;

	//UE_LOG(LogTemp, Warning, TEXT("Owner: %s"), *GetOwner()->GetName());

	for (FEquipmentSlot& Slot : EquipmentSlots.Slots)
	{
		if (Slot.Equipment)
		{
			if (GetOwner() != Slot.Equipment->GetOwner())
			{
				UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("Equipment Owner: %s, Invalidating equipment!"), *Slot.Equipment->GetOwner()->GetName()));
				UE_LOG(LogTemp, Warning, TEXT("%s"), *FString::Printf(TEXT("Owner: %s, Invalidating equipment!"), *GetOwner()->GetName()));
				Slot.Equipment = nullptr;
			}
		}
	}

	RefreshEquipment();
}

UE_ENABLE_OPTIMIZATION