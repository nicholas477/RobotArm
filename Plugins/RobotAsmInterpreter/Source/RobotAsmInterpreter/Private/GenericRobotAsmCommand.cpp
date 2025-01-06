// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericRobotAsmCommand.h"

void UGenericRobotAsmCommand::Tick(float DeltaTime)
{
	if (LastTickFrame == GFrameCounter)
	{
		return;
	}

	if (IsUnreachable())
	{
		MarkAsGarbage();
		TickCommand = false;
		return;
	}

	K2_Tick(DeltaTime);
	LastTickFrame = GFrameCounter;
}
