 // Fill out your copyright notice in the Description page of Project Settings.


#include "RobotAsmCommandFinishListener.h"

#include "RobotAsmInterpreterLibrary.h"

void URobotAsmCommandFinishListener::OnCommandFinish()
{
	URobotAsmInterpreterLibrary::RunCommandList_Index(Options);

	RemoveFromRoot();
	MarkAsGarbage();
}
