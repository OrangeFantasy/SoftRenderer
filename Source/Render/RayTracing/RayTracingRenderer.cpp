#include "Render/RayTracing/RayTracingRenderer.h"
#include "RayTracing/Ray.h"
#include "RayTracing/Scene.h"

#include <iomanip>

FRatTracingRenderer::FRatTracingRenderer(int32 InWidth, int32 InHeight, const FCamera& InCamera)
    : Width(InWidth), Height(InHeight), Camera(InCamera)
{
    FrameBuffer.resize((::std::size_t)(Width * Height));
    RenderThreadCount = ::std::thread::hardware_concurrency();
}

void FRatTracingRenderer::AddScene(FScene* InScene)
{
    Scene = InScene;
}

void FRatTracingRenderer::Render(int32 SPP)
{
    int32 OneThreadRows = Height / RenderThreadCount + 1;

    TArray<::std::thread> Workers;
    for (int32 i = 0; i < RenderThreadCount; ++i)
    {
        int32 BeginRow = i * OneThreadRows;
        int32 EndRow = FMath::Min(BeginRow + OneThreadRows, Height);

        Workers.emplace_back(::std::thread(&FRatTracingRenderer::RenderThread, this, BeginRow, EndRow, SPP));
    }

    for (::std::thread& Worker : Workers)
    {
        Worker.join();
    }
}

void FRatTracingRenderer::RenderThread(int32 Begin, int32 End, int32 SPP)
{
    float Scale = FMath::Tan(FMath::DegreesToRadians(Camera.GetCameraFov() / 2.0f));
    float AspectRatio = (float)Width / Height;

    for (int32 Row = Begin; Row < End; ++Row)
    {
        for (int32 Col = 0; Col < Width; ++Col)
        {
            int32 PixelIndex = Row * Width + Col;
            // FVector RTPixelVector;

            for (int32 SPPIndex = 0; SPPIndex < SPP; ++SPPIndex)
            {
                float X = (2 * (Col + FMath::RandomFloat()) / Width - 1) * Scale * AspectRatio;
                float Y = (1 - 2 * (Row + FMath::RandomFloat()) / Height - 1) * Scale;

                FVector Direction = FVector(-X, Y, 1.0f).GetSafeNormal();
                FRay Ray = FRay(Camera.GetCameraPosition(), Direction);
                // Scene->RayTracing(Ray, 0);
                FrameBuffer[PixelIndex] = Scene->RayTracing(Ray, 0);
            }
        }
        Mutex.lock();
        ++RowComplatedNum;
        UpdateProgressBar((float)RowComplatedNum / Height);
        Mutex.unlock();
    }
}

void FRatTracingRenderer::UpdateProgressBar(float Val)
{
    int32 BarWidth = 70;

    FString BarString;
    BarString += AUTO_TEXT("[");

    int32 Position = static_cast<int32>(BarWidth * Val);
    for (int32 i = 0; i < BarWidth; ++i)
    {
        if (i < Position)
        {
            BarString += AUTO_TEXT("=");
        }
        else if (i == Position)
        {
            BarString += AUTO_TEXT(">");
        }
        else
        {
            BarString += AUTO_TEXT(" ");
        }
    }
    BarString += AUTO_TEXT("] ") + ::std::to_wstring(Val * 100.0f) + AUTO_TEXT("%\r");
    FDebugString::Printf(BarString);
}
