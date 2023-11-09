#pragma once

#include "CoreTypes.h"
#include "Geometry/BoundingBox.h"

class FGeometry;
struct FHitResult;
struct FRay;

struct FBVHNode
{
    FBVHNode *Left, *Right;

    FGeometry* Object;
    FBoundingBox BoundingBox;
    float Area;

public:
    FBVHNode();
    ~FBVHNode() noexcept;
};

class FBoundingVolumeHierarchy
{
public:
    FBoundingVolumeHierarchy(TArray<FGeometry*> Primitives);
    ~FBoundingVolumeHierarchy() noexcept;

    FBVHNode* BuildBVH(TArray<FGeometry*>& Primitives, int32 Start, int32 End);

    void LineTrace(FHitResult& OutHitResult, const FRay& Ray);
    void Sample(FHitResult& OutHitResultm, float& OutPDF);

private:
    void LineTrace(FHitResult& OutHitResult, const FBVHNode* Node, const FRay& Ray);
    void Sample(FHitResult& OutHitResult, float& OutPDF, const FBVHNode* Node, float P);

private:
    FBVHNode* Root;

public:
    void Print();
};
