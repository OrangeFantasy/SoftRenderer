#pragma once

#include "CoreTypes.h"
#include "Geometry/BoundingBox.h"

class FGeometry
{
public:
    FGeometry() = default;
    virtual ~FGeometry() = default;

    virtual FBoundingBox GetBoundingBox() const = 0;
    virtual float GetArea() const = 0;
};
