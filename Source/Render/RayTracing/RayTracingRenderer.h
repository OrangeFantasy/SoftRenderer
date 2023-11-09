#pragma once

#include "CoreTypes.h"
#include "Render/Camera.h"

#include <mutex>

struct FViewport;
class FScene;

class FRatTracingRenderer
{
public:
    FRatTracingRenderer(int32 InWidth, int32 InHeight, const FCamera& InCamera);

    void AddScene(FScene* InScene);
    void Render(int32 SPP);

    // const FColor* GetFrameBuffer() const { return FrameBuffer.data(); }
    const FVector* GetFrameBuffer() const { return FrameBuffer.data(); }

private:
    void RenderThread(int32 Begin, int32 End, int32 SPP);

    void UpdateProgressBar(float Val);

private:
    FScene* Scene;
    FCamera Camera;
    // TArray<FColor> FrameBuffer;
    TArray<FVector> FrameBuffer;

    int32 Width;
    int32 Height;

    ::std::mutex Mutex;
    int32 RenderThreadCount;
    int32 RowComplatedNum = 0;
};
