#pragma once

#include "Render/Renderer.h"
#include "Render/Camera.h"

#include <mutex>

struct FRay;
struct FHitResult;
class FBoundingVolumeHierarchy;
class FGeometry;

class FRayTracingRenderer : public FRenderer
{
public:
    FRayTracingRenderer(int32 InWidth, int32 InHeight, const FCamera& InCamera);
    ~FRayTracingRenderer();

    void AddMesh(FGeometry* Mesh);

    void BuildBVH();
    void Render(int32 SPP, bool bMultiThread = true);

    // const FColor* GetFrameBuffer() const { return FrameBuffer.data(); }
    const FVector* GetFrameBuffer() const { return FrameBuffer.data(); }

private:
    void RenderThread(int32 Begin, int32 End, int32 SPP);

    FVector RayTracing(const FRay& Ray, int32 Depth);
    FVector Shade(const FHitResult& Hit, const FVector& Wo);
    void SampleLight(FHitResult& OutHit, float& OutPdf);

    void UpdateProgressBar(float Val);

private:
    // Camera and meshes.
    FCamera Camera;
    TArray<FGeometry*> Meshes;

    // Render target.
    TArray<FVector> FrameBuffer;

    // Render setting.
    FBoundingVolumeHierarchy* BVH = nullptr;
    float RussianRoulette = 0.8f;
    int32 Width;
    int32 Height;

    // Multi thread.
    ::std::mutex Mutex;
    int32 RenderThreadCount;
    int32 RowComplatedNum = 0;
};
