// Copyright Epic Games, Inc. All Rights Reserved.

#include "RobotAsmInterpreter.h"

#if WITH_EDITOR
#include "Logging/MessageLog.h"
#include "RobotAsmSettings.h"
#include "GenericRobotAsmCommand.h"
#include "Misc/UObjectToken.h"
#endif

#define LOCTEXT_NAMESPACE "FRobotAsmInterpreterModule"

void FRobotAsmInterpreterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FRobotAsmInterpreterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#if WITH_EDITOR
UE_DISABLE_OPTIMIZATION
void FRobotAsmInterpreterModule::CheckCommandConflicts()
{
	FMessageLog MessageLogger = FMessageLog("BlueprintLog");

	TMultiMap<FString, UObject*> CommandNameMap;
	for (TSubclassOf<UObject> Command : URobotAsmSettings::Get()->Commands)
	{
		if (Command)
		{
			UObject* CommandObject = Command->GetDefaultObject();
			if (UGenericRobotAsmCommand* GenericCommand = Cast<UGenericRobotAsmCommand>(CommandObject))
			{
				const FString CommandName = GenericCommand->Command.ToLower();
				CommandNameMap.Add(CommandName, GenericCommand);
			}
		}
	}

	TArray<TSharedRef<FTokenizedMessage>> ErrorMessages;

	TSet<FString> CommandNames;
	{
		TArray<FString> CommandNamesArray;
		CommandNameMap.GenerateKeyArray(CommandNamesArray);
		for (const FString& CommandName : CommandNamesArray)
		{
			CommandNames.Add(CommandName);
		}
	}


	for (const FString& CommandName : CommandNames)
	{
		TArray<UObject*> FoundCommands;
		CommandNameMap.MultiFind(CommandName, FoundCommands);

		if (FoundCommands.Num() > 1)
		{
			for (UObject* FoundCommand : FoundCommands)
			{
				FText ErrorMsg = FText::Format(INVTEXT("Assembly command {0} has conflicting command name: {1}"), FText::FromString(FoundCommand ? FoundCommand->GetClass()->GetName() : ""), FText::FromString(CommandName));
				TSharedRef<FTokenizedMessage> Message = FTokenizedMessage::Create(EMessageSeverity::Error, ErrorMsg);
				Message->SetMessageLink(FUObjectToken::Create(FoundCommand));

				ErrorMessages.Add(Message);
			}
		}
	}

	if (ErrorMessages.Num() > 0)
	{
		MessageLogger.AddMessages(ErrorMessages);
		MessageLogger.Open(EMessageSeverity::Error);
	}
}
UE_ENABLE_OPTIMIZATION
#endif

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FRobotAsmInterpreterModule, RobotAsmInterpreter)