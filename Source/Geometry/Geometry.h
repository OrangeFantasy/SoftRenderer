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

    virtual FBoundingBox GetBoundingBox() const = 0;
    virtual float GetArea() const = 0;
    virtual bool IsEmission() const = 0;

    virtual void BuildBVH() {}
    virtual void LineTrace(FHitResult& OutHitResult, const FRay& Ray) = 0;
    virtual void Sample(FHitResult& OutHitResult, float& OutPdf) = 0;
};
