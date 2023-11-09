#include "RayTracing/BoundingVolumeHierarchy.h"

#include <algorithm>
#include "Geometry/Geometry.h"
#include "RayTracing/HitResult.h"

// ********************
//       BVH Node
// ********************

FBVHNode::FBVHNode() : Left(nullptr), Right(nullptr), Object(nullptr), BoundingBox(FBoundingBox()), Area(0.0f) {}

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

FBoundingVolumeHierarchy::FBoundingVolumeHierarchy(TArray<FGeometry*> Primitives)
{
    for (FGeometry* Primitive : Primitives)
    {
        Primitive->BuildBVH();
    }

    Root = BuildBVH(Primitives, 0, Primitives.size());
}

FBoundingVolumeHierarchy::~FBoundingVolumeHierarchy() noexcept
{
    if (Root != nullptr)
    {
        delete Root;
        Root = nullptr;
    }
}

FBVHNode* FBoundingVolumeHierarchy::BuildBVH(TArray<FGeometry*>& Primitives, int32 Begin, int32 End)
{
    if (Begin >= End)
    {
        return nullptr;
    }

    // [Left, Right)
    // Right - Left == 1: Leaf node.

    FBVHNode* BVHNode = new FBVHNode();
    if (End - Begin == 1)
    {
        BVHNode->Object = Primitives[Begin];
        BVHNode->BoundingBox = Primitives[Begin]->GetBoundingBox();
        BVHNode->Area = Primitives[Begin]->GetArea();
    }
    else
    {
        FBoundingBox CentroidBoundingBox;
        for (int32 i = Begin; i < End; ++i)
        {
            CentroidBoundingBox |= Primitives[i]->GetBoundingBox().Centroid();
        }

        int32 Dim = CentroidBoundingBox.MaxAxis();
        ::std::sort(Primitives.begin() + Begin, Primitives.begin() + End,
            [&Dim](FGeometry* A, FGeometry* B) { return A->GetBoundingBox().Centroid()[Dim] < B->GetBoundingBox().Centroid()[Dim]; });

        // [Left, Right) -> [Left, Mid) | [Mid, Right)
        int32 Mid = (Begin + End) / 2;
        BVHNode->Left = BuildBVH(Primitives, Begin, Mid);
        BVHNode->Right = BuildBVH(Primitives, Mid, End);
        BVHNode->BoundingBox = BVHNode->Left->BoundingBox | BVHNode->Right->BoundingBox;
        BVHNode->Area = BVHNode->Left->Area + BVHNode->Right->Area;
    }
    return BVHNode;
}

void FBoundingVolumeHierarchy::LineTrace(FHitResult& OutHitResult, const FRay& Ray)
{
    LineTrace(OutHitResult, Root, Ray);
}

void FBoundingVolumeHierarchy::LineTrace(FHitResult& OutHitResult, const FBVHNode* Node, const FRay& Ray)
{
    if (Node != nullptr && Node->BoundingBox.IsIntersecting(Ray))
    {
        // leaf node.
        if (Node->Object != nullptr)
        {
            Node->Object->LineTrace(OutHitResult, Ray);
            return;
        }

        FHitResult LeftHit, RightHit;
        LineTrace(LeftHit, Node->Left, Ray);
        LineTrace(RightHit, Node->Right, Ray);

        OutHitResult = LeftHit.Time < RightHit.Time ? LeftHit : RightHit;
    }
}

void FBoundingVolumeHierarchy::Sample(FHitResult& OutHitResultm, float& OutPDF)
{
    float P = FMath::Sqrt(FMath::RandomFloat()) * Root->Area;
    Sample(OutHitResultm, OutPDF, Root, P);
    OutPDF /= Root->Area;
}

void FBoundingVolumeHierarchy::Sample(FHitResult& OutHitResult, float& OutPDF, const FBVHNode* Node, float P)
{
    if (Node->Object != nullptr)
    {
        Node->Object->Sample(OutHitResult, OutPDF);
        OutPDF *= Node->Area;
        return;
    }
    P < Node->Left->Area ? Sample(OutHitResult, OutPDF, Node->Left, P) //
                         : Sample(OutHitResult, OutPDF, Node->Right, P - Node->Left->Area);
}

static int32 NodeNum = 0;

void PreOrderTraversal(FBVHNode* Root, int32 Depth)
{
    if (Root)
    {
        if (Root->Object != nullptr)
        {
            ++NodeNum;
        }

        FString DebugString;
        for (int32 i = 0; i < Depth; ++i)
        {
            DebugString += AUTO_TEXT("  ");
        }
        DebugString += AUTO_TEXT("Flag\n");

        FDebugString::Printf(DebugString);
        PreOrderTraversal(Root->Left, Depth + 1);
        PreOrderTraversal(Root->Right, Depth + 1);
    }
}

void FBoundingVolumeHierarchy::Print()
{
    PreOrderTraversal(Root, 1);

    FString DebugString = AUTO_TEXT("Node Num: ") + std::to_wstring(NodeNum) + AUTO_TEXT("\n");
    FDebugString::Printf(DebugString);
}
