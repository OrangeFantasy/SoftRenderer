#pragma once

#include "CoreTypes.h"

struct FRay
{
    FVector Origin;
    FVector Direction;

    float Time;
    float Tmin;
    float Tmax;

    FRay() : Origin(FVector::ZeroVector), Direction(FVector(1.f, 0.f, 0.f)), Time(0.f), Tmin(0.f), Tmax(FLOAT_MAX) {}
    FRay(const FVector& InOrigin, const FVector& InDirection, float InTime = 0.f)
        : Origin(InOrigin), Direction(InDirection), Time(InTime), Tmin(0.f), Tmax(FLOAT_MAX)
    {
    }

    FVector GetLocation(float T) const
    {
        return Origin + Direction * T;
    }
};
