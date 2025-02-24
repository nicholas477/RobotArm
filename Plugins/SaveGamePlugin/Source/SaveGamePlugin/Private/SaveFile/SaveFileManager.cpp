// Copyright Alex Stevens (@MilkyEngineer). All Rights Reserved.


#include "SaveFile/SaveFileManager.h"

#include "SaveGameSettings.h"
#include "SaveGamePlugin.h"
#include "SaveGameSystem.h"
#include "PlatformFeatures.h"
#include "Json.h"

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

FJsonSaveFileManager::FJsonSaveFileManager(const FString& InSaveFileName)
	: FSaveFileManager(InSaveFileName)
{
}

void FJsonSaveFileManager::ReadData(const TArray<uint8>& File)
{
	FileMap.Empty();
	const FString JsonString = FString(File.Num() / sizeof(UTF8CHAR), (UTF8CHAR*)File.GetData());

	TSharedPtr<FJsonObject> Json = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (FJsonSerializer::Deserialize(Reader, Json))
	{
		for (const auto& Field : Json->Values)
		{
			TSharedPtr<FJsonObject>* FieldObject;
			if (Field.Value->TryGetObject(FieldObject))
			{
				const FName FieldName = FName(Field.Key);
				TArray<uint8> FieldData;

				FMemoryWriter Archive(FieldData);
				TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Archive);
				if (FJsonSerializer::Serialize(FieldObject->ToSharedRef(), Writer))
				{
					FileMap.Add(FieldName, FieldData);
				}
			}
		}
	}
}

void FJsonSaveFileManager::WriteData(TArray<uint8>& Data)
{
	TSharedPtr<FJsonObject> OutJsonObject = MakeShareable(new FJsonObject);
	for (const TPair<FName, TArray<uint8>>& File : FileMap)
	{
		const FString JsonString = FString(File.Value.Num() / sizeof(UTF8CHAR), (UTF8CHAR*)File.Value.GetData());
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

		TSharedPtr<FJsonObject> JsonFileObject;
		if (FJsonSerializer::Deserialize(Reader, JsonFileObject))
		{
			OutJsonObject->SetObjectField(File.Key.ToString(), JsonFileObject);
		}
	}

	FMemoryWriter Archive(Data);
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Archive);
	FJsonSerializer::Serialize(OutJsonObject.ToSharedRef(), Writer);
}
