#pragma once

#include "CoreTypes.h"

struct FBoundingBox;
struct FHitResult;
struct FRay;

class FGeometry
{
public:
    FGeometry() = default;
    virtual ~FGeometry() = default;

    virtual void BuildBVH() {}
    virtual FBoundingBox GetBoundingBox() const = 0;

    virtual float GetArea() const = 0;
    virtual void LineTrace(FHitResult& OutHitResult, const FRay& Ray) = 0;
};
