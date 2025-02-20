// Copyright Alex Stevens (@MilkyEngineer). All Rights Reserved.

#pragma once

#include "SaveGameSerializer.h"

#include "SaveGameSettings.h"
#include "SaveGameFunctionLibrary.h"
#include "SaveGameObject.h"
#include "SaveGameSubsystem.h"
#include "SaveGameVersion.h"
#include "SaveGamePlugin.h"

#include "SaveGameSystem.h"
#include "PlatformFeatures.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "Misc/UObjectToken.h"
#endif

#define LEVEL_SUBPATH_PREFIX TEXT("PersistentLevel.")

UE_DISABLE_OPTIMIZATION

template<bool bLoading>
FORCEINLINE_DEBUGGABLE void SerializeCompressedData(FArchive& Ar, TArray<uint8>& Data)
{
	check(Ar.IsLoading() == bLoading);
	
	int64 UncompressedSize;

	if (!bLoading)
	{
		UncompressedSize = Data.Num();
	}

	Ar << UncompressedSize;
	
	if (bLoading)
	{
		Data.SetNumUninitialized(UncompressedSize);
	}

	//Ar.SerializeCompressed(Data.GetData(), UncompressedSize, NAME_Zlib);
	Ar.Serialize(Data.GetData(), UncompressedSize);
}

template <bool bIsLoading, bool bIsTextFormat>
TSaveGameSerializer<bIsLoading, bIsTextFormat>::TSaveGameSerializer(USaveGameSubsystem* InSaveGameSubsystem)
	: SaveGameSubsystem(InSaveGameSubsystem)
	, Archive(Data)
	, ProxyArchive(Archive)
	, Formatter(ProxyArchive)
	, StructuredArchive(Formatter)
	, RootSlot(StructuredArchive.Open())
	, RootRecord(RootSlot.EnterRecord())
	, VersionOffset(0)
{
	static_cast<FArchive&>(ProxyArchive).SetIsTextFormat(bIsTextFormat);

	// Ensure that we're using the latest save game version
	Archive.UsingCustomVersion(FSaveGameVersion::GUID);
}

template <bool bIsLoading, bool bIsTextFormat>
bool TSaveGameSerializer<bIsLoading, bIsTextFormat>::Save()
{
	check(!bIsLoading);

	TRACE_BOOKMARK(TEXT("Begin: SaveGame[%s]"), bIsTextFormat ? TEXT("Text") : TEXT("Binary"));

	ON_SCOPE_EXIT
	{
		TRACE_BOOKMARK(TEXT("End: SaveGame[%s]"), bIsTextFormat ? TEXT("Text") : TEXT("Binary"));
	};
	
	if (ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem())
	{
		SerializeHeader();

		bool bFoundMapSlot;
		check(SaveGameSubsystem.IsValid());
		const UWorld* World = SaveGameSubsystem->GetWorld();
		FStructuredArchiveSlot MapSlot = EnterMapSlot(World, bFoundMapSlot);

		SerializeActors(MapSlot);
		SerializeDestroyedActors(MapSlot);

		if (!bIsTextFormat)
		{
			// Store the version position so that we can serialize it in the header
			VersionOffset = Archive.Tell();
		}
		
		SerializeVersions();

		if (!bIsTextFormat)
		{
			// We've updated the VersionOffset, let's go back to the start and rewrite the header
			Archive.Seek(0);
			SerializeHeader();
		}

		// Be sure to close this, as you'll be missing closed braces for JSON archives
		StructuredArchive.Close();
		
		if (!bIsTextFormat && !bIsLoading)
		{
			// Compress the save game data
			TArray<uint8> CompressedData;
			FSaveGameMemoryArchive CompressorArchive(CompressedData);
			SerializeCompressedData<false>(CompressorArchive, Data);

			if (GetDefault<USaveGameSettings>()->bEnableSaving)
			{
				return SaveSystem->SaveGame(false, *GetSaveName(), 0, CompressedData);
			}
		}
		
		if (GetDefault<USaveGameSettings>()->bEnableSaving)
		{
			return SaveSystem->SaveGame(false, *GetSaveName(), 0, Data);
		}
	}

	return false;
}

template <bool bIsLoading, bool bIsTextFormat>
bool TSaveGameSerializer<bIsLoading, bIsTextFormat>::Load(bool LoadMap)
{
	check(bIsLoading && !bIsTextFormat);

	TRACE_BOOKMARK(TEXT("Begin: LoadSaveGame[%s]"), bIsTextFormat ? TEXT("Text") : TEXT("Binary"));
	
	if (!GetDefault<USaveGameSettings>()->bEnableLoading)
	{
		//OnMapLoad(SaveGameSubsystem->GetWorld());
		return false;
	}

	TArray<uint8> CompressedData;
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (SaveSystem && SaveSystem->LoadGame(false, *GetSaveName(), 0, CompressedData))
	{
		// Decompress the loaded save game data
		FSaveGameMemoryArchive CompressorArchive(CompressedData);
		SerializeCompressedData<true>(CompressorArchive, Data);
		
		SerializeHeader();
		
		{
			const uint64 InitialPosition = Archive.Tell();

			// After serializing versions, go back to initial position
			ON_SCOPE_EXIT
			{
				Archive.Seek(InitialPosition);
			};

			Archive.Seek(VersionOffset);
			SerializeVersions();
		}

		if (LoadMap)
		{
			// If we don't have a map, we should bail
			if (MapName.IsEmpty())
			{
				return false;
			}

			check(SaveGameSubsystem.IsValid());
			UWorld* World = SaveGameSubsystem->GetWorld();

			if (World->IsInSeamlessTravel())
			{
				return false;
			}

			// When our map has loaded, call the OnMapLoad method
			FCoreUObjectDelegates::PostLoadMapWithWorld.AddThreadSafeSP(this, &TSaveGameSerializer::OnMapLoad);
			World->SeamlessTravel(MapName, true);

			return true;
		}
		else
		{
			OnMapLoad(SaveGameSubsystem->GetWorld());

			return true;
		}
	}

	return false;
}

template <bool bIsLoading, bool bIsTextFormat>
FString TSaveGameSerializer<bIsLoading, bIsTextFormat>::GetSaveName()
{
	FString SaveName = TEXT("SaveGame");

	if (bIsTextFormat)
	{
		SaveName += TEXT(".json");
	}

	return SaveName;
}

template<bool bIsLoading, bool bIsTextFormat>
FString TSaveGameSerializer<bIsLoading, bIsTextFormat>::GetMapName(const UWorld* World)
{
	return World->GetOutermost()->GetLoadedPath().GetPackageName();
}

template<bool bIsLoading, bool bIsTextFormat>
FStructuredArchiveSlot TSaveGameSerializer<bIsLoading, bIsTextFormat>::EnterMapSlot(const UWorld* World, bool& FoundMapSlot)
{
	FoundMapSlot = false;
	const FArchiveFieldName LevelsFieldName(TEXT("Levels"));

	int32 NumLevels = -1;
	FStructuredArchive::FMap LevelMap = RootRecord.EnterMap(LevelsFieldName, NumLevels);
	const FString SerializeMapName = GetMapName(World);

	if (bIsLoading)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Is loading: %s, Num levels in save: %d"), bIsLoading ? L"true" : L"false", NumLevels));

		FString SlotName = SerializeMapName;
		for (int i = 0; i < NumLevels; ++i)
		{
			auto Slot = LevelMap.EnterElement(SlotName);
			if (SlotName == SerializeMapName || i == (NumLevels - 1))
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Is loading: " + FString(bIsLoading ? L"true" : L"false") + ", Map name: " + SlotName);
				FoundMapSlot = SlotName == SerializeMapName;
				return Slot;
			}
			SlotName = SerializeMapName;
		}

		FoundMapSlot = false;
		auto Slot = LevelMap.EnterElement(SlotName);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Is loading: " + FString(bIsLoading ? L"true" : L"false") + ", Map name: " + SlotName);
		return Slot;
	}
	else
	{
		FoundMapSlot = true;
		FString SlotName = SerializeMapName;
		auto Slot = LevelMap.EnterElement(SlotName);

		UE_LOG(LogTemp, Warning, TEXT("Is loading : false, Map name : %s"), *SlotName);
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Is loading: false, Map name: " + SlotName);

		return Slot;
	}
}

template <bool bIsLoading, bool bIsTextFormat>
void TSaveGameSerializer<bIsLoading, bIsTextFormat>::OnMapLoad(UWorld* World)
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	check(SaveGameSubsystem->GetWorld() == World);
	bool bFoundMapSlot;
	FStructuredArchiveSlot MapSlot = EnterMapSlot(World, bFoundMapSlot);

	if (bFoundMapSlot)
	{
		// Actually serialize the actors
		SerializeActors(MapSlot);
		SerializeDestroyedActors(MapSlot);
	}

	SaveGameSubsystem->OnLoadCompleted();
	
	TRACE_BOOKMARK(TEXT("End: LoadSaveGame[%s]"), bIsTextFormat ? TEXT("Text") : TEXT("Binary"));
}

template <bool bIsLoading, bool bIsTextFormat>
void TSaveGameSerializer<bIsLoading, bIsTextFormat>::SerializeHeader()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SaveGame_SerializeHeader);
	
	// If we already have a map name, don't change it
	if (!bIsLoading && MapName.IsEmpty())
	{
		check(SaveGameSubsystem.IsValid());
		const UWorld* World = SaveGameSubsystem->GetWorld();

		MapName = World->GetOutermost()->GetLoadedPath().GetPackageName();
	}
	
	RootRecord << SA_VALUE(TEXT("Map"), MapName);
	
	FEngineVersion EngineVersion;
	FPackageFileVersion PackageVersion;

	if (!bIsLoading)
	{
		EngineVersion = FEngineVersion::Current();
		PackageVersion = GPackageFileUEVersion;
	}

	RootRecord << SA_VALUE(TEXT("EngineVersion"), EngineVersion);

	if (!bIsTextFormat)
	{
		// This doesn't have a structured archive serialize method
		Archive << PackageVersion;
		
		// We're a binary archive, so let's serialize where the version is
		// so that we can read it before loading anything
		RootRecord << SA_VALUE(TEXT("VersionsOffset"), VersionOffset);
	}

	if (bIsLoading)
	{
		Archive.SetEngineVer(EngineVersion);
		Archive.SetUEVer(PackageVersion);
	}
}

static void LogSpawnActorError(const FString& ActorName, AActor* ConflictingActor)
{
	UE_LOG(LogSaveGamePlugin, Warning, TEXT("Tried to spawn actor: %s, but actor already exists in the map!"), *ActorName);

#if WITH_EDITOR
	FMessageLog MessageLog("PIE");

	FText ErrorText;
	if (ConflictingActor)
	{
		ErrorText = FText::Format(INVTEXT("Save System: Failed to spawn actor {0}"), FText::FromString(ConflictingActor->GetName()));
	}

	MessageLog.Error()->AddToken(FUObjectToken::Create(ConflictingActor, ErrorText));
	//MessageLog.Open(EMessageSeverity::Error);
#endif
}

template<bool bIsLoading, bool bIsTextFormat>
void TSaveGameSerializer<bIsLoading, bIsTextFormat>::SerializeActors(FStructuredArchive::FSlot& Slot)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SaveGame_SerializeActors);

	if (true)
	{
		return;
	}
	
	// This serialize method assumes that we don't have any streamed/sub levels
	check(SaveGameSubsystem.IsValid());
	UWorld* World = SaveGameSubsystem->GetWorld();
	const FTopLevelAssetPath LevelAssetPath(World->GetCurrentLevel()->GetPackage()->GetFName(), World->GetCurrentLevel()->GetOuter()->GetFName());

	int32 NumActors;
	TArray<AActor*> Actors;
	TMap<FGuid, AActor*> SpawnIDs;
	
	const uint64 ActorsPosition = Archive.Tell();
	const FArchiveFieldName ActorsFieldName(TEXT("Actors"));
	
	if (bIsLoading)
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_SaveGame_InitializeActors);
		
		// Iterate through our live actors so that we can map their SpawnIDs
		for (const TWeakObjectPtr<AActor>& ActorPtr : SaveGameSubsystem->SaveGameActors)
		{
			AActor* Actor = ActorPtr.Get();
			if (IsValid(Actor) && Actor->Implements<USaveGameSpawnActor>())
			{
				const FGuid SpawnID = ISaveGameSpawnActor::Execute_GetSpawnID(Actor);

				if (SpawnID.IsValid())
				{
					SpawnIDs.Add(SpawnID, Actor);
				}
			}
		}
		
		FStructuredArchive::FMap ActorMap = Slot.EnterAttribute(ActorsFieldName).EnterMap(NumActors);

		Actors.SetNumZeroed(NumActors);

		// Iterate through the saved actors and spawn or find their live equivalent
		for (int32 ActorIdx = 0; ActorIdx < NumActors; ++ActorIdx)
		{
			AActor*& Actor = Actors[ActorIdx];

			// Populate our actors list with spawned actors or level references to actors
			SerializeActor(ActorMap, Actor, [&](const FString& ActorName, const FSoftClassPath& Class, const FGuid& SpawnID, FStructuredArchive::FSlot&)
			{
				ensureAlways(!ActorName.IsEmpty());

				if (Class.IsNull())
				{
					// This is a loaded actor (is a level actor), let's find it
					Actor = FindObjectFast<AActor>(World->GetCurrentLevel(), *ActorName);
				}
				else if (SpawnID.IsValid() && SpawnIDs.Contains(SpawnID))
				{
					Actor = SpawnIDs[SpawnID];
				}
				else
				{
					UClass* ActorClass = Class.TryLoadClass<AActor>();

					bool SpawnActor = true;
					if (ActorClass->ImplementsInterface(USaveGameSpawnActor::StaticClass()))
					{
						SpawnActor = ISaveGameSpawnActor::Execute_SpawnIfNotExists(ActorClass->ClassDefaultObject);
					}

					if (SpawnActor)
					{
						if (AActor* ConflictingActor = FindObjectFast<AActor>(World->GetCurrentLevel(), *ActorName))
						{
							LogSpawnActorError(ActorName, ConflictingActor);
						}
						else
						{
							// This is a spawned actor, let's spawn it
							FActorSpawnParameters SpawnParameters;

							// If we were handling levels, specify it here
							SpawnParameters.OverrideLevel = World->GetCurrentLevel();
							SpawnParameters.Name = *ActorName;
							SpawnParameters.bNoFail = true;

							Actor = World->SpawnActor(ActorClass, nullptr, nullptr, SpawnParameters);

							if (SpawnID.IsValid() && Actor->Implements<USaveGameSpawnActor>())
							{
								ISaveGameSpawnActor::Execute_SetSpawnID(Actor, SpawnID);
							}
						}
					}
				}

				if (SpawnID.IsValid())
				{
					const FString ActorSubPath = LEVEL_SUBPATH_PREFIX + ActorName;
					
					// We potentially have a spawned actor that other actors reference
					// If the name has changed, be sure to redirect the old actor path to the new one
					ProxyArchive.AddRedirect(FSoftObjectPath(LevelAssetPath, ActorSubPath), FSoftObjectPath(Actor));
				}
				
				//check(IsValid(Actor));
			});
		}
	}
	else
	{
		NumActors = SaveGameSubsystem->SaveGameActors.Num();
	}
	
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_SaveGame_SerializeActorData);
		
		if (bIsLoading && !bIsTextFormat)
		{
			// Go back to the start of the actor data
			Archive.Seek(ActorsPosition);
		}
		
		FStructuredArchive::FMap ActorMap = Slot.EnterAttribute(ActorsFieldName).EnterMap(NumActors);

		auto ActorsIt = SaveGameSubsystem->SaveGameActors.CreateConstIterator();
		
		// Actually serialize the actor data and their properties
		for (int32 ActorIdx = 0; ActorIdx < NumActors; ++ActorIdx)
		{
			AActor* Actor;
			
			if (bIsLoading)
			{
				Actor = Actors[ActorIdx];
			}
			else
			{
				Actor = ActorsIt->Get();
				++ActorsIt;
			}

			//check(IsValid(Actor));
			if (!IsValid(Actor))
			{
				continue;
			}

			// Do the actual serialization of the properties
			SerializeActor(ActorMap, Actor, [&](const FString&, const FSoftClassPath&, const FGuid& SpawnID, FStructuredArchive::FSlot& ActorSlot)
			{
				Actor->SerializeScriptProperties(ActorSlot.EnterAttribute(TEXT("Properties")));

				FStructuredArchive::FSlot CustomDataSlot = ActorSlot.EnterAttribute(TEXT("Data"));
				FStructuredArchive::FRecord CustomDataRecord = CustomDataSlot.EnterRecord();

				// Encapsulate the record in something a Blueprint can access 
				FSaveGameArchive SaveGameArchive(CustomDataRecord, Actor);
								
				ISaveGameObject::Execute_OnSerialize(Actor, SaveGameArchive, bIsLoading);
			});
		}
	}
}

template <bool bIsLoading, bool bIsTextFormat>
void TSaveGameSerializer<bIsLoading, bIsTextFormat>::SerializeDestroyedActors(FStructuredArchive::FSlot& Slot)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SaveGame_SerializeDestroyedActors);
	
	check(SaveGameSubsystem.IsValid());
	const UWorld* World = SaveGameSubsystem->GetWorld();
	FString SerializeMapName = GetMapName(World);
	
	int32 NumDestroyedActors;

	if (!bIsLoading)
	{
		NumDestroyedActors = SaveGameSubsystem->DestroyedLevelActors.Num();
	}

	FStructuredArchive::FArray DestroyedActorsArray = Slot.EnterAttribute(TEXT("DestroyedActors")).EnterArray(NumDestroyedActors);

	if (bIsLoading)
	{
		// Allocate our expected number of actors
		SaveGameSubsystem->DestroyedLevelActors.Reset();
		SaveGameSubsystem->DestroyedLevelActors.Reserve(NumDestroyedActors);
	}

	auto DestroyedActorsIt = SaveGameSubsystem->DestroyedLevelActors.CreateConstIterator();
	for (int32 ActorIdx = 0; ActorIdx < NumDestroyedActors; ++ActorIdx)
	{
		FName ActorName;

		if (!bIsLoading)
		{
			// Only store the object name without the prefix and full path
			FString ActorSubPath = (*DestroyedActorsIt).GetSubPathString();
			ActorSubPath.RemoveFromStart(LEVEL_SUBPATH_PREFIX);
			ActorName = *ActorSubPath;

			++DestroyedActorsIt;
		}
		
		DestroyedActorsArray.EnterElement() << ActorName;

		if (bIsLoading)
		{
			// Find the live actor in the level
			if (AActor* DestroyedActor = FindObjectFast<AActor>(World->GetCurrentLevel(), ActorName))
			{
				// Be sure to add any valid destroyed actors back into the array for saving later!
				SaveGameSubsystem->DestroyedLevelActors.Add(DestroyedActor);
				
				DestroyedActor->Destroy();
			}
		}
	}
}

template <bool bIsLoading, bool bIsTextFormat>
void TSaveGameSerializer<bIsLoading, bIsTextFormat>::SerializeVersions()
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SaveGame_SerializeVersions);
	
	FCustomVersionContainer VersionContainer;
	
	if (!bIsLoading)
	{
		// Grab a copy of our archive's current versions
		VersionContainer = Archive.GetCustomVersions();
	}

	VersionContainer.Serialize(RootRecord.EnterField(TEXT("Versions")));

	if (bIsLoading)
	{
		// Assign our serialized versions
		Archive.SetCustomVersions(VersionContainer);
	}
}

template <bool bIsLoading, bool bIsTextFormat>
void TSaveGameSerializer<bIsLoading, bIsTextFormat>::SerializeActor(FStructuredArchive::FMap& ActorMap, AActor*& Actor, TFunction<void(const FString&, const FSoftClassPath&, const FGuid&, FStructuredArchive::FSlot&)>&& BodyFunction)
{
	QUICK_SCOPE_CYCLE_COUNTER(STAT_SaveGame_SerializeActor);
	
	FString ActorName;
	FSoftClassPath Class;
	FGuid SpawnID;

	if (!bIsLoading)
	{
		ActorName = Actor->GetName();
				
		if (!USaveGameFunctionLibrary::WasObjectLoaded(Actor))
		{
			// We're a spawned actor, stash the class
			Class = Actor->GetClass();
		}

		if (Actor->Implements<USaveGameSpawnActor>())
		{
			SpawnID = ISaveGameSpawnActor::Execute_GetSpawnID(Actor);
		}
	}

	FStructuredArchive::FSlot ActorSlot = ActorMap.EnterElement(ActorName);

	// If we have a class, we're a spawned actor
	if (TOptional<FStructuredArchive::FSlot> ClassSlot = ActorSlot.TryEnterAttribute(TEXT("Class"), !Class.IsNull()))
	{
		ClassSlot.GetValue() << Class;
	}

	// If we have a GUID, we're a spawn actor that needs to be mapped by GUID
	TOptional<FStructuredArchive::FSlot> GuidSlot = ActorSlot.TryEnterAttribute(TEXT("GUID"), SpawnID.IsValid());
	if (GuidSlot.IsSet())
	{
		GuidSlot.GetValue() << SpawnID;
	}

	uint64 DataSize;

	if (!bIsTextFormat)
	{
		// Pre-write how much data (in bytes) was serialized for this actor
		Archive << DataSize;
	}

	const uint64 BeginDataPosition = Archive.Tell();

	BodyFunction(ActorName, Class, SpawnID, ActorSlot);

	if (!bIsTextFormat)
	{
		if (bIsLoading)
		{
			// Skip our data and onto the next actor
			Archive.Seek(BeginDataPosition + DataSize);
		}
		else
		{
			const uint64 EndDataPosition = Archive.Tell();
			DataSize = EndDataPosition - BeginDataPosition;

			// Store the amount of data we've serialized (in bytes), back before the actual data
			Archive.Seek(BeginDataPosition - sizeof(DataSize));
			Archive << DataSize;

			// Go back to our current position
			Archive.Seek(EndDataPosition);
		}
	}
}

// Instantiate the permutations of TSaveGameSerializer

#if WITH_TEXT_ARCHIVE_SUPPORT
template TSaveGameSerializer<false, true>;
#endif

template TSaveGameSerializer<false, false>;
template TSaveGameSerializer<true, false>;

UE_ENABLE_OPTIMIZATION