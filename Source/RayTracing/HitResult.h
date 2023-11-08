#pragma once

#include "CoreTypes.h"

struct FHitResult
{
    class FGeometry* Object;
    struct FMaterial* Material;

    bool bHit;
    float Time;
    FVector Location;
    FVector Normal;
};
