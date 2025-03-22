// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MainMenuLevel.generated.h"

/**
 * Represents a selectable level from the main menu
 */
UCLASS(Blueprintable)
class MAINMENU_API UMainMenuLevel : public UObject
{
	GENERATED_BODY()
	
public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;

	// Lowest number = leftmost on list
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Main Menu Level")
	int32 LevelSortingIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Main Menu Level")
	TSoftObjectPtr<UWorld> Level;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Main Menu Level")
	TSoftObjectPtr<UTexture2D> LevelIcon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Main Menu Level")
	FText LevelName;

	UFUNCTION(BlueprintCallable, Category = "Main Menu Level")
	static TArray<TSoftObjectPtr<UMainMenuLevel>> GetMainMenuLevels();

	UFUNCTION(BlueprintCallable, Category = "Main Menu Level")
	static TArray<UMainMenuLevel*> LoadMainMenuLevels();
};
