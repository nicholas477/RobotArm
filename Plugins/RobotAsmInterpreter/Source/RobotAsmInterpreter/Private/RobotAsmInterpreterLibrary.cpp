// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotAsmInterpreterLibrary.h"

#include "RobotAsmSettings.h"
#include "RobotAsmCommandFinishListener.h"
#include "Commands/LabelCommand.h"
#include "Commands/GotoCommand.h"
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
				if (CommandClass == nullptr)
				{
					continue;
				}
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

void URobotAsmInterpreterLibrary::RunCommandList(const UObject* WorldContextObject, const FRunCommandOptions& Options)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		for (UObject* Command : Options.CommandList)
		{
			if (IsValid(Command) && Command->GetClass()->ImplementsInterface(URobotAsmCommandInterface::StaticClass()))
			{
				IRobotAsmCommandInterface::Execute_SetCommandWorld(Command, World);
			}
		}

		RunCommandList_Index(Options);
	}
}

void URobotAsmInterpreterLibrary::RunCommandList_Index(const FRunCommandOptions& Options)
{
	if (!Options.CommandList.IsValidIndex(Options.CommandIndex))
	{
		Options.OnFinish.ExecuteIfBound();
		return;
	}

	if (Options.ShouldStopDelegate.IsBound())
	{
		if (Options.ShouldStopDelegate.Execute())
		{
			Options.OnFinish.ExecuteIfBound();
			return;
		}
	}

	UObject* Command = Options.CommandList[Options.CommandIndex];

	if (IsValid(Command) && Command->GetClass()->ImplementsInterface(URobotAsmCommandInterface::StaticClass()))
	{
		if (Command->GetWorld() == nullptr || Command->GetWorld()->bIsTearingDown)
		{
			Options.OnFinish.ExecuteIfBound();
			return;
		}

		// GOTO command is a special case
		if (UGotoCommand* GotoCommand = Cast<UGotoCommand>(Command))
		{
			const FString& LabelName = GotoCommand->LabelName;
			int32 LabelIndex;
			if (FindLabelIndex(Options.CommandList, LabelName, LabelIndex) && GotoCommand->ShouldBranch(Options.CommandList, Options.State))
			{
				FRunCommandOptions NewOptions = Options;
				NewOptions.CommandIndex = LabelIndex;
				RunCommandList_Index(NewOptions);
			}
			else
			{
				// If we couldn't find a label then just skip this command
				FRunCommandOptions NewOptions = Options;
				NewOptions.CommandIndex += 1;

				RunCommandList_Index(NewOptions);
			}
			return;
		}
		
		URobotAsmCommandFinishListener* CommandFinishListener = NewObject<URobotAsmCommandFinishListener>();
		CommandFinishListener->AddToRoot();

		CommandFinishListener->Options = Options;
		CommandFinishListener->Options.CommandIndex += 1;

		static const FName OnCommandFinishFunctionName("OnCommandFinish");
		FOnCommandFinish OnCommandFinish;
		OnCommandFinish.BindUFunction(CommandFinishListener, OnCommandFinishFunctionName);

		UE_LOG(LogTemp, Warning, TEXT("Running command: Index: %d, Name: %s"), Options.CommandIndex, *Command->GetName());
		IRobotAsmCommandInterface::Execute_RunCommand(Command, Options.CommandList, OnCommandFinish, Options.State);
	}
}

bool URobotAsmInterpreterLibrary::FindLabelIndex(const TArray<UObject*>& CommandList, const FString& Label, int32& OutIndex)
{
	for (OutIndex = 0; OutIndex < CommandList.Num(); ++OutIndex)
	{
		const UObject* Command = CommandList[OutIndex];
		if (const ULabelCommand* LabelCommand = Cast<const ULabelCommand>(Command))
		{
			if (LabelCommand->LabelName == Label)
			{
				return true;
			}
		}
	}
	return false;
}

void URobotAsmInterpreterLibrary::WriteAssemblyStateVariable(FRobotAsmStateWrapper Wrapper, FName VariableName, int32 Value)
{
	Wrapper.State->Variables.FindOrAdd(VariableName) = Value;
}

int32 URobotAsmInterpreterLibrary::ReadAssemblyStateVariable(FRobotAsmStateWrapper Wrapper, FName VariableName)
{
	return Wrapper.State->Variables.FindOrAdd(VariableName);
}
UE_ENABLE_OPTIMIZATION
