// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuSettings.h"

#include "MainMenuFunctionLibrary.h"

UMainMenuFunctionLibrary* UMainMenuSettings::GetMainMenuFunctionLibrary()
{
    UMainMenuSettings* Settings = GetMainMenuSettings();
    if (ensure(Settings != nullptr))
    {
        if (Settings->MainMenuFunctionLibraryOverride != nullptr)
        {
            return Settings->MainMenuFunctionLibraryOverride->GetDefaultObject<UMainMenuFunctionLibrary>();
        }
        else
        {
            return UMainMenuFunctionLibrary::StaticClass()->GetDefaultObject<UMainMenuFunctionLibrary>();
        }
    }

    return nullptr;
}
