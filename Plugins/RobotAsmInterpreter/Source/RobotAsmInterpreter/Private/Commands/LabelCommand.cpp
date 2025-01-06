// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/LabelCommand.h"

ULabelCommand::ULabelCommand()
{
	Command = "label";
}

void ULabelCommand::RunCommand_Implementation(const TArray<UObject*>& CommandList, const FOnCommandFinish& OnFinish)
{
	OnFinish.ExecuteIfBound();
}

void ULabelCommand::ConstructCommand_Implementation(const TArray<FString>& Words)
{
	if (Words.Num() == 2)
	{
		LabelName = Words[1];
	}
}
