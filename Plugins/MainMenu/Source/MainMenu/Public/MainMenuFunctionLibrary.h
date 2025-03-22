// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MainMenuFunctionLibrary.generated.h"

class UMainMenuLevel;

/**
 * 
 */
UCLASS(Blueprintable)
class MAINMENU_API UMainMenuFunctionLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Main Menu Function Library")
	bool CompareMainMenuLevels(const UMainMenuLevel* Lhs, const UMainMenuLevel* Rhs) const;

	UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Main Menu Function Library")
	TArray<UMainMenuLevel*> SortMainMenuLevels(const TArray<UMainMenuLevel*>& Levels) const;
};
