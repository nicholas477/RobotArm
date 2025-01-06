// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotAsmInterpreterLibrary.h"

#include "RobotAsmSettings.h"
#include "RobotAsmCommandFinishListener.h"
#include "Containers/UnrealString.h"

UE_DISABLE_OPTIMIZATION
bool URobotAsmInterpreterLibrary::InterpretCode(const UObject* WorldContextObject, const FString& Code, FString& OutErrorString, TArray<UObject*>& OutCommands)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		OutCommands.Empty();

		TArray<FString> Lines;
		Code.ParseIntoArray(Lines, TEXT("\n"));

		for (FString& Line : Lines)
		{
			bool b;
			Line.TrimCharInline('\r', &b);
			Line.TrimCharInline('\n', &b);
			Line.ToLowerInline();


			TArray<FString> Words;
			Line.ParseIntoArray(Words, TEXT(" "));

			if (Words.Num() >= 1)
			{
				TSubclassOf<UObject> CommandClass = URobotAsmSettings::GetCommand(Words[0]);
				UObject* NewCommand = NewObject<UObject>(GetTransientPackage(), CommandClass);

				IRobotAsmCommandInterface::Execute_SetCommandWorld(NewCommand, World);
				IRobotAsmCommandInterface::Execute_ConstructCommand(NewCommand, Words);

				OutCommands.Add(NewCommand);
			}
		}

		return true;
	}

	return false;
}

void URobotAsmInterpreterLibrary::RunCommandList(const UObject* WorldContextObject, const TArray<UObject*>& CommandList, const FOnCommandFinish& OnFinish)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (UObject* Command : CommandList)
		{
			if (IsValid(Command) && Command->GetClass()->ImplementsInterface(URobotAsmCommandInterface::StaticClass()))
			{
				IRobotAsmCommandInterface::Execute_SetCommandWorld(Command, World);
			}
		}

		RunCommandList_Index(CommandList, OnFinish, 0);
	}
}

void URobotAsmInterpreterLibrary::RunCommandList_Index(const TArray<UObject*>& CommandList, const FOnCommandFinish& OnFinish, int32 CommandIndex)
{
	if (!CommandList.IsValidIndex(CommandIndex))
	{
		OnFinish.ExecuteIfBound();
		return;
	}

	UObject* Command = CommandList[CommandIndex];

	if (IsValid(Command) && Command->GetClass()->ImplementsInterface(URobotAsmCommandInterface::StaticClass()))
	{

		URobotAsmCommandFinishListener* CommandFinishListener = NewObject<URobotAsmCommandFinishListener>();
		CommandFinishListener->AddToRoot();

		CommandFinishListener->NextCommandIndex = CommandIndex + 1;
		CommandFinishListener->CommandList = CommandList;
		CommandFinishListener->OnFinish = OnFinish;

		static const FName OnCommandFinishFunctionName("OnCommandFinish");
		FOnCommandFinish OnCommandFinish;
		OnCommandFinish.BindUFunction(CommandFinishListener, OnCommandFinishFunctionName);

		IRobotAsmCommandInterface::Execute_RunCommand(Command, CommandList, OnCommandFinish);
	}
}
UE_ENABLE_OPTIMIZATION
