#include "Render/RayTracing/RayTracingRenderer.h"
#include "RayTracing/Ray.h"
#include "RayTracing/Scene.h"

#include <iomanip>
#include <opencv2/opencv.hpp>

FRayTracingRenderer::FRayTracingRenderer(int32 InWidth, int32 InHeight, const FCamera& InCamera)
    : Width(InWidth), Height(InHeight), Camera(InCamera)
{
    FrameBuffer.resize((::std::size_t)(Width * Height));
    RenderThreadCount = ::std::thread::hardware_concurrency();
}

void FRayTracingRenderer::AddScene(FScene* InScene)
{
    Scene = InScene;
}

void FRayTracingRenderer::Render(int32 SPP, bool bMultiThread)
{
    int32 OneThreadRows = Height / RenderThreadCount + 1;

    if (bMultiThread)
    {
        TArray<::std::thread> Workers;
        for (int32 i = 0; i < RenderThreadCount; ++i)
        {
            int32 BeginRow = i * OneThreadRows;
            int32 EndRow = FMath::Min(BeginRow + OneThreadRows, Height);

            Workers.emplace_back(::std::thread(&FRayTracingRenderer::RenderThread, this, BeginRow, EndRow, SPP));
        }

        for (::std::thread& Worker : Workers)
        {
            Worker.join();
        }
    }
    else
    {
        RenderThread(0, Height, SPP);
    }

#pragma warning(disable : 4267)
    cv::Mat Image(Height, Width, CV_32FC3);
    for (int32 Row = 0; Row < Image.rows; ++Row)
    {
        for (int32 Col = 0; Col < Image.cols; ++Col)
        {
            int32 Index = Row * Width + Col;
            Image.at<cv::Vec3f>(Row, Col)[0] = FrameBuffer[Index].X;
            Image.at<cv::Vec3f>(Row, Col)[1] = FrameBuffer[Index].Y;
            Image.at<cv::Vec3f>(Row, Col)[2] = FrameBuffer[Index].Z;
        }
    }
    cv::imshow("Image", Image);
    Image.convertTo(Image, CV_8UC3, 255.0f);
    cv::imwrite("./RTImage.png", Image);
    cv::waitKey();
#pragma warning(default : 4267)
}

void FRayTracingRenderer::RenderThread(int32 Begin, int32 End, int32 SPP)
{
    float Scale = FMath::Tan(FMath::DegreesToRadians(Camera.GetCameraFov() * 0.5f));
    float AspectRatio = Width / (float)Height;

    for (int32 Row = Begin; Row < End; ++Row)
    {
        for (int32 Col = 0; Col < Width; ++Col)
        {
            FVector RTPixelVector;
            for (int32 SPPIndex = 0; SPPIndex < SPP; ++SPPIndex)
            {
                float X = (2 * (Col + FMath::RandomFloat()) / Width - 1) * Scale * AspectRatio;
                float Y = (1 - 2 * (Row + FMath::RandomFloat()) / Height) * Scale;

                FVector Direction = FVector(-X, Y, 1.0f).GetSafeNormal();
                FRay Ray = FRay(Camera.GetCameraLocation(), Direction);

                RTPixelVector += Scene->RayTracing(Ray, 0);
            }

            int32 PixelIndex = Row * Width + Col;
            FrameBuffer[PixelIndex] = RTPixelVector / SPP;
        }
        Mutex.lock();
        ++RowComplatedNum;
        UpdateProgressBar((float)RowComplatedNum / Height);
        Mutex.unlock();
    }
}

void FRayTracingRenderer::UpdateProgressBar(float Val)
{
    int32 BarWidth = 70;

    std::cout << "[";
    int32 Position = static_cast<int32>(BarWidth * Val);
    for (int32 i = 0; i < BarWidth; ++i)
    {
        if (i < Position)
        {
            std::cout << "=";
        }
        else if (i == Position)
        {
            std::cout << ">";
        }
        else
        {
            std::cout << " ";
        }
    }
    std::cout << "] " << std::fixed << std::setprecision(2) << Val * 100.0f << "%\n";
    std::cout.flush();
}
