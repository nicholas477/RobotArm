// Copyright Alex Stevens (@MilkyEngineer). All Rights Reserved.

#include "SaveGameSubsystem.h"

#include "SaveGameFunctionLibrary.h"
#include "SaveGameObject.h"
#include "SaveGameSerializer.h"
#include "SaveFile/SaveFileManager.h"

#include "EngineUtils.h"

void USaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &ThisClass::OnWorldInitialized);
	FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &ThisClass::OnActorsInitialized);
	FWorldDelegates::OnWorldCleanup.AddUObject(this, &ThisClass::OnWorldCleanup);

	BinaryFileManager = MakeShared<FSaveFileManager>("SaveGame");

#if !UE_BUILD_SHIPPING && WITH_TEXT_ARCHIVE_SUPPORT
	JsonFileManager = MakeShared<FSaveFileManager>("SaveGame.json");
#endif

	// This example doesn't handle streaming levels, but if we did, we'd use a combination of
	// FWorldDelegates::LevelAddedToWorld and FWorldDelegates::PreLevelRemovedFromWorld
	// In these, we'd store the current state of actors within that level

	OnWorldInitialized(GetWorld(), UWorld::InitializationValues());
}

void USaveGameSubsystem::Deinitialize()
{
#if !UE_BUILD_SHIPPING && WITH_TEXT_ARCHIVE_SUPPORT
	JsonFileManager.Reset();
#endif

	BinaryFileManager.Reset();

	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	FWorldDelegates::OnWorldInitializedActors.RemoveAll(this);
	FWorldDelegates::OnWorldCleanup.RemoveAll(this);
	
	FWorldDelegates::LevelAddedToWorld.RemoveAll(this);
	FWorldDelegates::PreLevelRemovedFromWorld.RemoveAll(this);
}

bool USaveGameSubsystem::Save()
{
	//if (CurrentSerializer == nullptr)
	{
		const TSharedRef<TSaveGameSerializer<false>> BinarySerializer = MakeShared<TSaveGameSerializer<false>>(this, BinaryFileManager);
		CurrentSerializer = BinarySerializer.ToSharedPtr();
	}
	bool bSuccess = CurrentSerializer->Save();
	bSuccess &= BinaryFileManager->SaveGameFile();

#if !UE_BUILD_SHIPPING && WITH_TEXT_ARCHIVE_SUPPORT
	// This is for debug purposes only, we want to use binary serialization for smallest file sizes
	TSaveGameSerializer<false, true> TextSerializer(this, JsonFileManager);
	bSuccess &= TextSerializer.Save();
	bSuccess &= JsonFileManager->SaveGameFile();
#endif
	
	return bSuccess;
}

bool USaveGameSubsystem::Load(bool LoadMap)
{
	BinaryFileManager->LoadGameFile();

#if !UE_BUILD_SHIPPING && WITH_TEXT_ARCHIVE_SUPPORT
	JsonFileManager->LoadGameFile();
#endif

	//if (CurrentSerializer == nullptr)
	{
		const TSharedRef<TSaveGameSerializer<true>> BinarySerializer = MakeShared<TSaveGameSerializer<true>>(this, BinaryFileManager);
		CurrentSerializer = BinarySerializer.ToSharedPtr();

	}
	return CurrentSerializer->Load(LoadMap);
}

bool USaveGameSubsystem::IsLoadingSaveGame() const
{
	return CurrentSerializer.IsValid();
}

void USaveGameSubsystem::OnWorldInitialized(UWorld* World, const UWorld::InitializationValues)
{
	if (!IsValid(World) || GetWorld() != World)
	{
		return;
	}
	
	World->AddOnActorPreSpawnInitialization(FOnActorSpawned::FDelegate::CreateUObject(this, &ThisClass::OnActorPreSpawn));
	World->AddOnActorDestroyedHandler(FOnActorDestroyed::FDelegate::CreateUObject(this, &ThisClass::OnActorDestroyed));
}

void USaveGameSubsystem::OnActorsInitialized(const FActorsInitializedParams& Params)
{
	if (!IsValid(Params.World) || GetWorld() != Params.World)
	{
		return;
	}
	
	for (TActorIterator<AActor> It(Params.World); It; ++It)
	{
		AActor* Actor = *It;
		if (IsValid(Actor) && Actor->Implements<USaveGameObject>())
		{
			SaveGameActors.Add(Actor);
		}
	}
}

void USaveGameSubsystem::OnWorldCleanup(UWorld* World, bool, bool)
{
	if (!IsValid(World) || GetWorld() != World)
	{
		return;
	}
	
	SaveGameActors.Reset();
	DestroyedLevelActors.Reset();
}

void USaveGameSubsystem::OnActorPreSpawn(AActor* Actor)
{
	if (IsValid(Actor) && Actor->Implements<USaveGameObject>())
	{
		SaveGameActors.Add(Actor);
	}
}

void USaveGameSubsystem::OnActorDestroyed(AActor* Actor)
{
	SaveGameActors.Remove(Actor);

	if (USaveGameFunctionLibrary::WasObjectLoaded(Actor))
	{
		DestroyedLevelActors.Add(Actor);
	}
}

void USaveGameSubsystem::OnLoadCompleted()
{
	CurrentSerializer = nullptr;
}
