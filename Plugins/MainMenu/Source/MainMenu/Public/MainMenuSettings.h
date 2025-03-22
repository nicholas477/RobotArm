// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MainMenuSettings.generated.h"

class UMainMenuFunctionLibrary;

/**
 * 
 */
UCLASS(config = Game, defaultconfig)
class MAINMENU_API UMainMenuSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Main Menu Settings", meta=(AllowedClasses = "/Script/Engine.Texture2D"))
	TArray<FSoftObjectPath> MainMenuBackgrounds;

	UPROPERTY(Config, BlueprintReadWrite, EditAnywhere, Category = "Main Menu Settings")
	TSubclassOf<UMainMenuFunctionLibrary> MainMenuFunctionLibraryOverride;

	UFUNCTION(BlueprintPure, Category = "Main Menu Settings")
	static UMainMenuFunctionLibrary* GetMainMenuFunctionLibrary();

	UFUNCTION(BlueprintPure, Category = "Main Menu Settings")
	static UMainMenuSettings* GetMainMenuSettings() { return GetMutableDefault<UMainMenuSettings>(); }
};
