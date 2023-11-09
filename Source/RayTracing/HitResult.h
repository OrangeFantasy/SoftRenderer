#pragma once

#include "CoreTypes.h"

struct FHitResult
{
    bool bHit = false;
    float Time = 0.0f;
    FVector Location = FVector::ZeroVector;
    FVector Normal = FVector::ZeroVector;
    FVector Emission = FVector::ZeroVector;

    class FGeometry* Object = nullptr;
    struct FMaterial* Material = nullptr;
};
