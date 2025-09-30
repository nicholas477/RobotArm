// Fill out your copyright notice in the Description page of Project Settings.


#include "RobotArmWorldSettings.h"

#if WITH_EDITOR
#include "Grid/GridPreviewActor.h"

ARobotArmWorldSettings* ARobotArmWorldSettings::GetRobotArmWorldSettings(const UObject* WorldContextObject)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		return Cast<ARobotArmWorldSettings>(World->GetWorldSettings());
	}
	return nullptr;
}

void ARobotArmWorldSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropName = PropertyChangedEvent.GetMemberPropertyName();
	if (PropName == GET_MEMBER_NAME_CHECKED(ThisClass, GridActor) 
		|| PropName == GET_MEMBER_NAME_CHECKED(ThisClass, bGridActorEditable))
	{
		if (GridActor != nullptr)
		{
			((AGridPreviewActor*)GridActor)->SetEditable(bGridActorEditable);
		}
	}

	if (PropName == GET_MEMBER_NAME_CHECKED(ThisClass, GridSettings))
	{
		if (GridActor != nullptr)
		{
			GridActor->SetActorLocation(GridSettings.GridOffset);
		}

		OnGridSettingsChanged.Broadcast(this, GridSettings);
	}
}
#endif
