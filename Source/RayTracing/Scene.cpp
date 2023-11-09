#include "RayTracing/Scene.h"

#include "Geometry/Geometry.h"
#include "RayTracing/BoundingVolumeHierarchy.h"
#include "RayTracing/Ray.h"
#include "RayTracing/HitResult.h"
#include "Material/Material.h"

FScene::~FScene()
{
    if (BVH != nullptr)
    {
        delete BVH;
        BVH = nullptr;
    }
}

void FScene::AddMesh(FGeometry* Mesh)
{
    Meshes.emplace_back(Mesh);
}

void FScene::BuildBVH()
{
    BVH = new FBoundingVolumeHierarchy(Meshes);
}

FVector FScene::RayTracing(const FRay& Ray, int32 Depth)
{
    FHitResult HitResult;
    BVH->LineTrace(HitResult, Ray);
    if (HitResult.bHit)
    {
        return Shade(HitResult, -Ray.Direction);
    }
    return FVector::ZeroVector;
}

FVector FScene::Shade(const FHitResult& Hit, const FVector& Wo)
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

void FScene::SampleLight(FHitResult& OutHit, float& OutPdf)
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
