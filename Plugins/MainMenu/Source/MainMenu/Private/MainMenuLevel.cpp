// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuLevel.h"

#include "Engine/AssetManager.h"
#include "Engine/Blueprint.h"

FPrimaryAssetId UMainMenuLevel::GetPrimaryAssetId() const
{
	return FPrimaryAssetId("MainMenuLevel", GetFName());
}

TArray<TSoftObjectPtr<UMainMenuLevel>> UMainMenuLevel::GetMainMenuLevels()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	TArray<FSoftObjectPath> MapObjects;
	AssetManager.GetPrimaryAssetPathList(FPrimaryAssetType("MainMenuLevel"), MapObjects);

	TArray<TSoftObjectPtr<UMainMenuLevel>> Maps;
	for (const FSoftObjectPath& MapObject : MapObjects)
	{
		Maps.AddUnique(TSoftObjectPtr(MapObject));
	}

	return Maps;
}

TArray<UMainMenuLevel*> UMainMenuLevel::LoadMainMenuLevels()
{
	UAssetManager& AssetManager = UAssetManager::Get();

	TArray<FSoftObjectPath> MapObjects;
	AssetManager.GetPrimaryAssetPathList(FPrimaryAssetType("MainMenuLevel"), MapObjects);

	TArray<UMainMenuLevel*> Maps;
	for (const FSoftObjectPath& MapObject : MapObjects)
	{
		UObject* MainMenuObject = MapObject.TryLoad();
		if (UMainMenuLevel* LoadedMap = Cast<UMainMenuLevel>(MainMenuObject))
		{
			Maps.AddUnique(LoadedMap);
		}
		else if (UBlueprint* LoadedBlueprint = Cast<UBlueprint>(MainMenuObject))
		{
			if (UMainMenuLevel* LoadedMapClass = Cast<UMainMenuLevel>(LoadedBlueprint->GeneratedClass->ClassDefaultObject))
			{
				Maps.AddUnique(LoadedMapClass);
			}
		}
		else if (UBlueprintGeneratedClass* LoadedBlueprintClass = Cast<UBlueprintGeneratedClass>(MainMenuObject))
		{
			if (UMainMenuLevel* LoadedMapClass = Cast<UMainMenuLevel>(LoadedBlueprintClass->ClassDefaultObject))
			{
				Maps.AddUnique(LoadedMapClass);
			}
		}
	}

	return Maps;
}
