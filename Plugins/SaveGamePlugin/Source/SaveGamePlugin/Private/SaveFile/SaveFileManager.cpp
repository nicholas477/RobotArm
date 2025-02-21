// Copyright Alex Stevens (@MilkyEngineer). All Rights Reserved.


#include "SaveFile/SaveFileManager.h"

#include "SaveGameSettings.h"
#include "SaveGamePlugin.h"
#include "SaveGameSystem.h"
#include "PlatformFeatures.h"

//template<bool bLoading>
//FORCEINLINE_DEBUGGABLE void SerializeCompressedData(FArchive& Ar, TArray<uint8>& Data)
//{
//	check(Ar.IsLoading() == bLoading);
//	
//	int64 UncompressedSize;
//
//	if (!bLoading)
//	{
//		UncompressedSize = Data.Num();
//	}
//
//	Ar << UncompressedSize;
//	
//	if (bLoading)
//	{
//		Data.SetNumUninitialized(UncompressedSize);
//	}
//
//	//Ar.SerializeCompressed(Data.GetData(), UncompressedSize, NAME_Zlib);
//	Ar.Serialize(Data.GetData(), UncompressedSize);
//}

FSaveFileManager::FSaveFileManager(const FString& InSaveFileName)
	: SaveFileName(InSaveFileName)
{
}

bool FSaveFileManager::SaveGameFile()
{
	TArray<uint8> Data;
	WriteData(Data);

	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();

	if (SaveSystem && GetDefault<USaveGameSettings>()->bEnableSaving)
	{
		return SaveSystem->SaveGame(false, *SaveFileName, 0, Data);
	}

	return false;
}

bool FSaveFileManager::LoadGameFile()
{
	if (!GetDefault<USaveGameSettings>()->bEnableLoading)
	{
		return false;
	}

	TArray<uint8> Data;
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (SaveSystem && SaveSystem->LoadGame(false, *SaveFileName, 0, Data))
	{
		ReadData(Data);
		return true;
	}

	return false;
}

bool FSaveFileManager::FileExists(FName FileName)
{
	return FileMap.Find(FileName) != nullptr;
}

TArray<uint8>& FSaveFileManager::GetFileData(FName FileName)
{
	return FileMap.FindOrAdd(FileName);
}

void FSaveFileManager::ReadData(const TArray<uint8>& File)
{
	FMemoryReader Reader(File);
	Reader << FileMap;
}

void FSaveFileManager::WriteData(TArray<uint8>& Data)
{
	FMemoryWriter Writer(Data);
	Writer << FileMap;
}
