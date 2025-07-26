// Copyright Alex Stevens (@MilkyEngineer). All Rights Reserved.

#pragma once

#if WITH_TEXT_ARCHIVE_SUPPORT
#include "Serialization/Formatters/JsonArchiveOutputFormatter.h"
#endif

#include "Templates/ChooseClass.h"
#include "SaveGameProxyArchive.h"

class USaveGameSubsystem;

class FSaveGameSerializer :  public TSharedFromThis<FSaveGameSerializer>
{
public:
	virtual bool Save() = 0;
	virtual bool Load(bool LoadMap = false) = 0;

	virtual ~FSaveGameSerializer() = default;
};

/**
 * The class that manages serializing the world.
 *
 * Archive data structured like so:
 * - Header
 *		- Map Name
 *		- Engine Versions
 * - Actors
 *		- Actor Name #1:
 *			- Class: If spawned
 *			- SpawnID: If implements ISaveGameSpawnActor
 *			- SaveGame Properties
 *			- Data written by ISaveGameObject::OnSerialize
 *		- ...
 * - Destroyed Level Actors
 *		- Actor Name #1
 *		- ...
 * - Versions
 *		- Version:
 *			- ID
 *			- Version Number
 *		- ...
 */
template<bool bIsLoading, bool bIsTextFormat = false>
class TSaveGameSerializer final : public FSaveGameSerializer
{
	using FSaveGameMemoryArchive = typename TChooseClass<bIsLoading, FMemoryReader, FMemoryWriter>::Result;

	static_assert(!bIsLoading || !bIsTextFormat, "This serializer hasn't been implemented for text based loading, only saving!");
	static_assert(WITH_TEXT_ARCHIVE_SUPPORT || !bIsTextFormat, "Engine isn't compiled with text archive support, cannot use text based TSaveGameSerializer");
	
	using FSaveGameFormatter = typename TChooseClass<bIsTextFormat && WITH_TEXT_ARCHIVE_SUPPORT,
		typename TChooseClass<bIsLoading, FBinaryArchiveFormatter, FJsonArchiveOutputFormatter>::Result,
		FBinaryArchiveFormatter>::Result;
	
public:
	TSaveGameSerializer(USaveGameSubsystem* InSaveGameSubsystem, TSharedPtr<class FSaveFileManager> InFileManager);

	virtual bool Save() override;
	virtual bool Load(bool LoadMap = false) override;

private:
	static FString GetMapName(const UWorld* World);

	FStructuredArchiveSlot EnterMapSlot(const UWorld* World, bool& FoundMapSlot);
	
	void OnMapLoad(UWorld* World);

	/** Serializes information about the archive, like Map Name, and position of versioning information */
	void SerializeHeader();

	/**
	 * Serializes all of the actors that the SaveGameSubsystem is keeping track of.
	 * On load, it will also pre-spawn any actors and map any actors with Spawn IDs
	 * before running the actual serialization step.
	 */
	void SerializeActors(FStructuredArchive::FSlot& Slot);

	/** Serializes any destroyed level actors. On load, level actors will exist again, so this will re-destroy them */
	void SerializeDestroyedActors(FStructuredArchive::FSlot& Slot);

	/**
	 * Serialized at the end of the archive, the versions are useful for marshaling old data.
	 * These also contain the versions added by USaveGameFunctionLibrary::UseCustomVersion.
	 */
	void SerializeVersions();

	/**
	 * Serializes the actor's data into the structured archive.
	 * This data always comprises of the actor's object name, and optionally its:
	 * - Class: If the actor was spawned (so that it can be spawned again)
	 * - SpawnID: If the actor implements ISaveGameSpawnActor. A unique identifier to map the data back to an already
	 *				spawned actor (like the player's character)
	 *
	 * It also takes a lambda function that can optionally do some work or serialization. Ultimately, once this
	 * lambda function is complete, SerializeActor will automatically seek the archive to the end of the actor's data.
	 * 
	 * @param ActorMap The structured map that the actor data will be written to
	 * @param Actor The live actor that will be serialized
	 * @param BodyFunction A lambda function that will optionally do some work, whether that be serializing or spawning
	 */
	void SerializeActor(FStructuredArchive::FMap& ActorMap, AActor*& Actor, TFunction<void(const FString&, const FSoftClassPath&, const FGuid&, FStructuredArchive::FSlot&)>&& BodyFunction);

	/** Serializes an actor's components into the actor slot. Matches components by name */
	void SerializeActorComponents(FStructuredArchive::FSlot& ActorSlot, AActor* Actor);

	void SerializeComponent(FStructuredArchive::FSlot& ComponentSlot, UActorComponent* Component);
	
	const TWeakObjectPtr<USaveGameSubsystem> SaveGameSubsystem;
	TSharedPtr<class FSaveFileManager> FileManager;

	struct FArchiver
	{
		FArchiver()
			: Archive(Data)
			, ProxyArchive(Archive)
			, Formatter(ProxyArchive)
			, StructuredArchive(Formatter)
			, RootSlot(StructuredArchive.Open())
		{
			static_cast<FArchive&>(ProxyArchive).SetIsTextFormat(bIsTextFormat);
		}
		TArray<uint8> Data;
		FSaveGameMemoryArchive Archive;
		TSaveGameProxyArchive<bIsLoading> ProxyArchive;
		FSaveGameFormatter Formatter;
		FStructuredArchive StructuredArchive;
		FStructuredArchive::FSlot RootSlot;
	};
	FArchiver RootArchiver;

	struct FMapArchiver : public FArchiver
	{
		FName MapFileName;
	};
	TUniquePtr<FMapArchiver> MapArchiver;

	FStructuredArchive::FRecord RootRecord;

	FString MapName;
	uint64 VersionOffset;

	FString FilenameExtension;
};
