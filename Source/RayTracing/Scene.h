#pragma once

#include "CoreTypes.h"

struct FRay;
struct FHitResult;
class FBoundingVolumeHierarchy;
class FGeometry;

class FScene
{
public:
    FScene() = default;
    ~FScene();

    void AddMesh(FGeometry* Mesh);

    void BuildBVH();

    FVector RayTracing(const FRay& Ray, int32 Depth = 3);

private:
    FVector Shade(const FHitResult& Hit, const FVector& Wo);
    void SampleLight(FHitResult& OutHit, float& OutPdf);

private:
    TArray<FGeometry*> Meshes;

    FBoundingVolumeHierarchy* BVH = nullptr;
    float RussianRoulette = 0.8f;
};