#pragma once

#include "Geometry/Geometry.h"
#include "Geometry/Vertex.h"

struct FTexture;
class FTriangle;
class FBoundingVolumeHierarchy;

class FMesh
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

private:
    FString Name;
    TArray<FVertex> Vertices;
    TArray<FVector3i> Indices;

    FTexture* Texture = nullptr;

    // ********************
    //      Transform
    // ********************

public:
    void SetTransform(
        const FVector& InTranslation = FVector(0.0f), const FVector& InRotation = FVector(0.0f), const FVector& InScale = FVector(1.0f));
    void UpdateModelMatrix();

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
    void BuildBVH();

private:
    void DestroyBVH() noexcept;

private:
    FBoundingVolumeHierarchy* BVH = nullptr;

    TArray<FGeometry*> RTPrimitives;
    FBoundingBox BoundingBox;
    float Area = 0.0f;
};
