// Fill out your copyright notice in the Description page of Project Settings.


#include "GenericRobotAsmCommand.h"

#include "RobotAsmInterpreter.h"
#include "RobotAsmSettings.h"

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

#if WITH_EDITOR
void UGenericRobotAsmCommand::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	//const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	const FName MemberPropertyName = (PropertyChangedEvent.MemberProperty != nullptr) ? PropertyChangedEvent.MemberProperty->GetFName() : NAME_None;

	if (MemberPropertyName == GET_MEMBER_NAME_CHECKED(UGenericRobotAsmCommand, Command))
	{
		if (URobotAsmSettings::Get()->Commands.Contains(GetClass()))
		{
			FRobotAsmInterpreterModule::CheckCommandConflicts();
		}
	}
}
#endif
