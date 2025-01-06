// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/GotoCommand.h"

UGotoCommand::UGotoCommand()
{
	Command = "goto";
}

void UGotoCommand::ConstructCommand_Implementation(const TArray<FString>& Words)
{
	if (Words.Num() == 2)
	{
		LabelName = Words[1];
	}
}
