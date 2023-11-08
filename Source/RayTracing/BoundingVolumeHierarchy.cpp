#include "RayTracing/BoundingVolumeHierarchy.h"

#include "Geometry/Geometry.h"
#include "Geometry/BoundingBox.h"

#include <algorithm>

// ********************
//       BVH Node
// ********************

FBVHNode::FBVHNode() : Left(nullptr), Right(nullptr), Geometry(nullptr), BoundingBox(FBoundingBox()) /**, Area(0.0f)*/ {}

FBVHNode::~FBVHNode()
{
    if (Left != nullptr)
    {
        delete Left;
        Left = nullptr;
    }
    if (Right != nullptr)
    {
        delete Right;
        Right = nullptr;
    }
}

// ********************
//         BVH
// ********************

FBoundingVolumeHierarchy::FBoundingVolumeHierarchy(TArray<FGeometry*> Geometries)
{
    Root = BuildBVH(Geometries, 0, Geometries.size() - 1);
}

FBoundingVolumeHierarchy::~FBoundingVolumeHierarchy() noexcept
{
    if (Root != nullptr)
    {
        delete Root;
        Root = nullptr;
    }
}

FBVHNode* FBoundingVolumeHierarchy::BuildBVH(TArray<FGeometry*>& Geometries, int32 Left, int32 Right)
{
    if (Left > Right)
    {
        return nullptr;
    }

    // [Left, Right]
    // Left == Right: Leaf node.

    FBVHNode* BVHNode = new FBVHNode();
    if (Left == Right)
    {
        BVHNode->Geometry = Geometries[Left];
        BVHNode->BoundingBox = Geometries[Left]->GetBoundingBox();
    }
    else
    {
        FBoundingBox CentroidBoundingBox;
        for (int32 i = Left; i <= Right; ++i)
        {
            CentroidBoundingBox |= Geometries[i]->GetBoundingBox().Centroid();
        }

        int32 Dim = CentroidBoundingBox.MaxAxis();
        ::std::sort(Geometries.begin() + Left, Geometries.begin() + Right + 1,
            [&Dim](FGeometry* A, FGeometry* B) { return A->GetBoundingBox().Centroid()[Dim] < B->GetBoundingBox().Centroid()[Dim]; });

        // [Left, Right] -> [Left, Mid] | [Mid + 1, Right]
        int32 Mid = (Left + Right) / 2;
        BVHNode->Left = BuildBVH(Geometries, Left, Mid);
        BVHNode->Right = BuildBVH(Geometries, Mid + 1, Right);
        BVHNode->BoundingBox = BVHNode->Left->BoundingBox | BVHNode->Right->BoundingBox;
    }
    return BVHNode;
}
