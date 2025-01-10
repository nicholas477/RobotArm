// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/LabelCommand.h"

ULabelCommand::ULabelCommand()
{
	Command = "label";

	CommandShortDescription = INVTEXT("Declare label");

	CommandDescription = INVTEXT("Creates a new label that can be jumped to using the jmp command.");
	CommandLatexDescription = INVTEXT("Creates a new label that can be jumped to using the \\textbf{jmp} command.");

	CommandLatexExample = INVTEXT("label label1\n\njmp label1");
}

void ULabelCommand::RunCommand_Implementation(const FRunCommandOptions& Options)
{
	Options.OnFinish.ExecuteIfBound();
}

void ULabelCommand::ConstructCommand_Implementation(const TArray<FString>& Words)
{
	if (Words.Num() == 2)
	{
		LabelName = Words[1];
	}
}
