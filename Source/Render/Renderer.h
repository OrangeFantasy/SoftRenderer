#pragma once

#include "CoreTypes.h"

enum ERendererType
{
    Rasterization,
    RayTracing
};

class FRenderer
{
public:
    FRenderer() = default;
    virtual ~FRenderer() = default;
};