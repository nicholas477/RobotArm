// Copyright Alex Stevens (@MilkyEngineer). All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"

// Manages the file directory inside of a save file
class FSaveFileManager : public TSharedFromThis<FSaveFileManager>
{
public:
	FSaveFileManager(const FString& InSaveFileName);
	virtual ~FSaveFileManager() = default;

	bool SaveGameFile();
	bool LoadGameFile();

	bool FileExists(FName FileName);
	TArray<uint8>& GetFileData(FName FileName);

protected:
	virtual void ReadData(const TArray<uint8>& File);
	virtual void WriteData(TArray<uint8>& Data);

	TMap<FName, TArray<uint8>> FileMap;
	FString SaveFileName;
};

class FJsonSaveFileManager final : public FSaveFileManager
{
public:
	FJsonSaveFileManager(const FString& InSaveFileName);
protected:
	virtual void ReadData(const TArray<uint8>& File);
	virtual void WriteData(TArray<uint8>& Data);
};