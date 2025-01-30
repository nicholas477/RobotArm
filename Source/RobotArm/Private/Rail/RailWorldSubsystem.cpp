// Fill out your copyright notice in the Description page of Project Settings.


#include "Rail/RailWorldSubsystem.h"

#include "Rail/RailConnectionComponent.h"

URailConnectionComponent* URailWorldSubsystem::GetAdjacentConnection(URailConnectionComponent* Connection, float DistThreshold)
{
    if (Connection == nullptr)
    {
        return nullptr;
    }

    const FVector ThisConnectionPos = Connection->GetComponentLocation();
    for (URailConnectionComponent* OtherConnection : Connections)
    {
        if (OtherConnection == Connection)
        {
            continue;
        }

        if (OtherConnection == nullptr)
        {
            continue;
        }

        const FVector OtherConnectionPos = OtherConnection->GetComponentLocation();
        const float Dist = FVector::Distance(ThisConnectionPos, OtherConnectionPos);

        if (Dist <= DistThreshold)
        {
            return OtherConnection;
        }
    }

    return nullptr;
}
