#pragma once

#include "CoreTypes.h"
#include "Geometry/BoundingBox.h" 

class FGeometry;

struct FBVHNode
{
    FBVHNode *Left, *Right;

    FGeometry* Geometry;
    FBoundingBox BoundingBox;
    // float Area;

public:
    FBVHNode();
    ~FBVHNode() noexcept;
};

class FBoundingVolumeHierarchy
{
public:
    FBoundingVolumeHierarchy(TArray<FGeometry*> Geometries);
    ~FBoundingVolumeHierarchy() noexcept;

    FBVHNode* BuildBVH(TArray<FGeometry*>& Geometries, int32 Start, int32 End);

private:
    FBVHNode* Root;
};
