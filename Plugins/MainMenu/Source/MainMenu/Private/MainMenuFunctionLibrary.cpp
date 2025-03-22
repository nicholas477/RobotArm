// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuFunctionLibrary.h"

#include "MainMenuLevel.h"

bool UMainMenuFunctionLibrary::CompareMainMenuLevels_Implementation(const UMainMenuLevel* Lhs, const UMainMenuLevel* Rhs) const
{
    if (Lhs != nullptr && Rhs != nullptr)
    {
        return Lhs->LevelSortingIndex < Rhs->LevelSortingIndex;
    }
    return false;
}

TArray<UMainMenuLevel*> UMainMenuFunctionLibrary::SortMainMenuLevels_Implementation(const TArray<UMainMenuLevel*>& Levels) const
{
    TArray<UMainMenuLevel*> RetVal = Levels;

    RetVal.Sort([this](const UMainMenuLevel& Lhs, const UMainMenuLevel& Rhs)
    {
        return CompareMainMenuLevels(&Lhs, &Rhs);
    });

    return RetVal;
}
