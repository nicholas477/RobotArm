// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "Grid/GridTypes.h"
#include "RobotArmWorldSettings.generated.h"

class UUserWidget;
class ARobotArmWorldSettings;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRobotArmGridSettingsChanged, ARobotArmWorldSettings*, WorldSettings, const FWorldGridSettings&, GridSettings);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Arm")
	bool AllowDeviceInteraction = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Arm")
	FWorldGridSettings GridSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Arm")
	TObjectPtr<AActor> GridActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Robot Arm")
	bool bGridActorEditable = false;

	UPROPERTY(BlueprintAssignable)
	FOnRobotArmGridSettingsChanged OnGridSettingsChanged;

	UFUNCTION(BlueprintPure, meta=(WorldContext = "WorldContextObject"))
	static ARobotArmWorldSettings* GetRobotArmWorldSettings(const UObject* WorldContextObject);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
};
