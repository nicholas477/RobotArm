// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/GotoCommand.h"

UGotoCommand::UGotoCommand()
{
	Command = "jmp";

	CommandShortDescription = INVTEXT("Jump");

	CommandDescription = INVTEXT("Unconditionally jumps to a label.");
	CommandLatexDescription = INVTEXT("Unconditionally jumps to a \\textbf{label}.");

	CommandLatexExample = INVTEXT("label label1\n\njmp label1");
}

bool UGotoCommand::ShouldBranch_Implementation(const TArray<UObject*>& CommandList, FRobotAsmStateWrapper State)
{
	return true;
}

void UGotoCommand::ConstructCommand_Implementation(const TArray<FString>& Words)
{
	if (Words.Num() >= 2)
	{
		LabelName = Words[1];
	}
}
