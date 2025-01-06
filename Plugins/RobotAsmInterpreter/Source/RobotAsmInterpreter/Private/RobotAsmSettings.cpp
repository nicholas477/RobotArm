// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotAsmSettings.h"

#include "GenericRobotAsmCommand.h"

TSubclassOf<UObject> URobotAsmSettings::GetCommand(const FString& CommandName)
{
	for (TSubclassOf<UObject> CommandClass : Get()->Commands)
	{
		if (CommandClass.Get() == nullptr)
		{
			continue;
		}

		UObject* Command = CommandClass->GetDefaultObject();
		if (UGenericRobotAsmCommand* GenericCommand = Cast<UGenericRobotAsmCommand>(Command))
		{
			if (GenericCommand->Command.ToLower() == CommandName.ToLower())
			{
				return CommandClass;
			}
		}
	}

	return TSubclassOf<UObject>();
}
