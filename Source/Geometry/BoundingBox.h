#pragma once

#include "CoreTypes.h"

struct FRay;

struct FBoundingBox
{
    FVector MinPoint;
    FVector MaxPoint;

public:
    FBoundingBox();
    FBoundingBox(const FVector& PointA, const FVector& PointB);

    static FBoundingBox Union(const FBoundingBox& A, const FBoundingBox& B);
    static FBoundingBox Union(const FBoundingBox& A, const FVector& V);

    FBoundingBox UnionWith(const FBoundingBox& Other) const;
    FBoundingBox UnionWith(const FVector& V) const;

    FBoundingBox operator|(const FBoundingBox& Other) const;
    FBoundingBox operator|(const FVector& V) const;

    FBoundingBox& operator|=(const FBoundingBox& Other);
    FBoundingBox& operator|=(const FVector& V);

    bool Contains(const FVector& V) const;

    FVector Centroid() const;
    FVector Diagonal() const;
    int32 MaxAxis() const;
    float SurfaceArea() const;

    bool IsIntersecting(const FRay& Ray) const;
};
