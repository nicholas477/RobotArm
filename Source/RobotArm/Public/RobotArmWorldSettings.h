// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "RobotArmWorldSettings.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class ROBOTARM_API ARobotArmWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Robot Arm")
	TSubclassOf<UUserWidget> ObjectiveWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Arm")
	bool ShowHud = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Arm")
	bool ShowDeviceListWidget = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Arm")
	bool ShowProgrammingWidget = true;
};
