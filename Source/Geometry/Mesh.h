#pragma once

#include "Geometry/Geometry.h"
#include "Geometry/Vertex.h"
#include "Geometry/BoundingBox.h"

struct FTexture;
struct FMaterial;
class FTriangle;
class FBoundingVolumeHierarchy;

class FMesh : public FGeometry
{
    // ********************
    //        Basic
    // ********************
public:
    FMesh();
    ~FMesh();

    void AddVertex(const FVertex& Vertex) { Vertices.emplace_back(Vertex); }
    void AddIndex(const FVector3i& Index) { Indices.emplace_back(Index); }

    const TArray<FVertex>& GetVertices() const { return Vertices; }
    const TArray<FVector3i>& GetIndices() const { return Indices; }

    void SetTexture(FTexture* InTexture) { Texture = InTexture; }
    const FTexture* GetTexture() const { return Texture; }

    void SetMaterial(FMaterial* InMaterial) { Material = InMaterial; }
    const FMaterial* GetMaterial() const { return Material; }

private:
    FString Name;
    TArray<FVertex> Vertices;
    TArray<FVector3i> Indices;

    FTexture* Texture = nullptr;
    FMaterial* Material = nullptr;

    // ********************
    //      Transform
    // ********************

public:
    void UpdateModelMatrix();
    void SetTransform(
        const FVector& InTranslation = FVector(0.0f), const FVector& InRotation = FVector(0.0f), const FVector& InScale = FVector(1.0f));
    void ApplyTransform(
        const FVector& InTranslation = FVector(0.0f), const FVector& InRotation = FVector(0.0f), const FVector& InScale = FVector(1.0f));

public:
    FMatrix4 ModelMatrix;

private:
    FVector Translation = FVector(0.0f);
    FVector Rotation = FVector(0.0f);
    FVector Scale = FVector(1.0f);

    // ********************
    //     Ray tracing
    // ********************

public:
    virtual FBoundingBox GetBoundingBox() const override { return BoundingBox; };
    virtual float GetArea() const override { return Area; };
    virtual bool IsEmission() const override;

    virtual void BuildBVH() override;
    virtual void LineTrace(FHitResult& OutHitResult, const FRay& Ray) override;
    virtual void Sample(FHitResult& OutHitResult, float& OutPdf) override;

private:
    void DestroyBVH() noexcept;

private:
    FBoundingVolumeHierarchy* BVH = nullptr;

    TArray<struct FGeometry*> RTPrimitives;
    FBoundingBox BoundingBox;
    float Area = 0.0f;
};
