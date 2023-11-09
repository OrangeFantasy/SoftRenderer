#include "Render/RayTracing/RayTracingRenderer.h"
#include "RayTracing/BoundingVolumeHierarchy.h"
#include "RayTracing/HitResult.h"
#include "RayTracing/Ray.h"
#include "Material/Material.h"
#include "Geometry/Geometry.h"

#include <iomanip>
#include <opencv2/opencv.hpp>

FRayTracingRenderer::FRayTracingRenderer(int32 InWidth, int32 InHeight, const FCamera& InCamera)
    : Width(InWidth), Height(InHeight), Camera(InCamera)
{
    FrameBuffer.resize((::std::size_t)(Width * Height));
    RenderThreadCount = ::std::thread::hardware_concurrency();
}

FRayTracingRenderer::~FRayTracingRenderer()
{
    if (BVH != nullptr)
    {
        delete BVH;
        BVH = nullptr;
    }
}

void FRayTracingRenderer::AddMesh(FGeometry* Mesh)
{
    Meshes.emplace_back(Mesh);
}

void FRayTracingRenderer::BuildBVH()
{
    BVH = new FBoundingVolumeHierarchy(Meshes);
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

                RTPixelVector += RayTracing(Ray, 0);
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

FVector FRayTracingRenderer::RayTracing(const FRay& Ray, int32 Depth)
{
    FHitResult HitResult;
    BVH->LineTrace(HitResult, Ray);
    if (HitResult.bHit)
    {
        return Shade(HitResult, -Ray.Direction);
    }
    return FVector::ZeroVector;
}

FVector FRayTracingRenderer::Shade(const FHitResult& Hit, const FVector& Wo)
{
    if (Hit.Material->IsEmission())
    {
        return Hit.Material->Emission;
    }

    FVector LoDirect;
    {
        FHitResult LightHit;
        float PDF = 0.0f;
        SampleLight(LightHit, PDF);

        FVector LightVector = LightHit.Location - Hit.Location;
        FVector LightDirection = LightVector.GetSafeNormal();

        FHitResult ObstacleHit;
        BVH->LineTrace(ObstacleHit, FRay(Hit.Location, LightDirection));

        if (ObstacleHit.Time - LightVector.Length() > -KINDA_SMALL_NUMBER)
        {
            FVector Fr = Hit.Material->Evaluate(LightDirection, Wo, Hit.Normal);
            float R2 = LightVector.Length();
            float CosA = FMath::Max(0.0f, FVector::DotProduct(LightDirection, Hit.Normal));
            float CosB = FMath::Max(0.0f, FVector::DotProduct(-LightDirection, LightHit.Normal));
            LoDirect = LightHit.Emission * Fr * CosA * CosB / (R2 * PDF);
        }
    }

    FVector LoIndirect = FVector::ZeroVector;
    if (FMath::RandomFloat() < RussianRoulette)
    {
        FVector Wi = Hit.Material->Sample(Wo, Hit.Normal);
        float PDF = Hit.Material->PDF(Wo, Wi, Hit.Normal);
        if (PDF > SMALL_NUMBER)
        {
            FHitResult NextHit;
            BVH->LineTrace(NextHit, FRay(Hit.Location, Wi));
            if (NextHit.bHit && !NextHit.Material->IsEmission())
            {
                FVector Fr = Hit.Material->Evaluate(Wi, Wo, Hit.Normal);
                float Cos = FMath::Max(0.0f, FVector::DotProduct(Wi, Hit.Normal));
                LoIndirect = Shade(NextHit, -Wi) * Fr * Cos / (PDF * RussianRoulette);
            }
        }
    }

    return LoDirect + LoIndirect;
}

void FRayTracingRenderer::SampleLight(FHitResult& OutHit, float& OutPdf)
{
    float EmissionArea = 0.0f;
    for (const FGeometry* Mesh : Meshes)
    {
        EmissionArea += (Mesh->IsEmission() ? Mesh->GetArea() : 0.0f);
    }

    float P = FMath::RandomFloat() * EmissionArea;
    EmissionArea = 0.0f;
    for (FGeometry* Mesh : Meshes)
    {
        if (Mesh->IsEmission())
        {
            EmissionArea += Mesh->GetArea();
            if (EmissionArea >= P)
            {
                Mesh->Sample(OutHit, OutPdf);
                return;
            }
        }
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
