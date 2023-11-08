#include "Geometry/Mesh.h"
#include "Geometry/Triangle.h"
#include "RayTracing/BoundingVolumeHierarchy.h"
#include "Mesh.h"

FMesh::FMesh() : Name(AUTO_TEXT("Mesh")) {}

FMesh::~FMesh()
{
    DestroyBVH();
}

void FMesh::SetTransform(const FVector& InTranslation, const FVector& InRotation, const FVector& InScale)
{
    Translation = InTranslation;
    Rotation = InRotation;
    Scale = InScale;
}

void FMesh::UpdateModelMatrix()
{
    // Rotation Matrix. Y X Z
    float Yaw = FMath::DegreesToRadians(Rotation.Y);
    float Pitch = FMath::DegreesToRadians(Rotation.X);
    float Roll = FMath::DegreesToRadians(Rotation.Z);

    float C1 = FMath::Cos(Yaw), C2 = FMath::Cos(Pitch), C3 = FMath::Cos(Roll);
    float S1 = FMath::Sin(Yaw), S2 = FMath::Sin(Pitch), S3 = FMath::Sin(Roll);

    FMatrix4 RotationMatrix = FMatrix4(                             //
        C1 * C3 + S1 * S2 * S3, C3 * S1 * S2 - C1 * S3, C2 * S1, 0, //
        C2 * S3, C2 * C3, -S2, 0,                                   //
        C1 * S2 * S3 - C3 * S1, C1 * C3 * S2 + S1 * S3, C1 * C2, 0, //
        0, 0, 0, 1                                                  //
    );

    // Scale Matrix.
    FMatrix4 ScaleMatrix = FMatrix4( //
        Scale.X, 0, 0, 0,            //
        0, Scale.Y, 0, 0,            //
        0, 0, Scale.Z, 0,            //
        0, 0, 0, 1                   //
    );

    // Translation Matrix.
    FMatrix4 TranslationMatrix = FMatrix4( //
        1, 0, 0, Translation.X,            //
        0, 1, 0, Translation.Y,            //
        0, 0, 1, Translation.Z,            //
        0, 0, 0, 1                         //
    );

    ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;
}

void FMesh::BuildBVH()
{
    for (const FVector3i& Index : Indices)
    {
        FTriangle* Triangle = new FTriangle(Vertices[Index.X], Vertices[Index.Y], Vertices[Index.Z], Material);
        BoundingBox |= Triangle->GetBoundingBox();
        Area += Triangle->GetArea();
        RTPrimitives.emplace_back(Triangle);
    }

    BVH = new FBoundingVolumeHierarchy(RTPrimitives);
    // BVH->Print();
}

void FMesh::LineTrace(FHitResult& OutHitResult, const FRay& Ray)
{
    if (BVH != nullptr)
    {
        BVH->LineTrace(OutHitResult, Ray);
    }
}

void FMesh::DestroyBVH() noexcept
{
    if (BVH != nullptr)
    {
        delete BVH;
        BVH = nullptr;
    }

    for (FGeometry* Primitive : RTPrimitives)
    {
        if (Primitive != nullptr)
        {
            delete Primitive;
            Primitive = nullptr;
        }
    }
}
