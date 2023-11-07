#pragma once

#include "CoreTypes.h"
#include "Geometry/Vertex.h"

struct FTexture;

struct FMesh
{
    FString Name;
    TArray<FVertex> Vertices;
    TArray<FVector3i> Indices;

    struct
    {
        FVector Position;
        FVector Rotation;
        FVector Scale;
    };
    FMatrix4 ModelMatrix;

    bool bVisible = true;

    FTexture* Texture = nullptr;

public:
    FMesh() : Name(AUTO_TEXT("Mesh")), Position(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), Scale(1.0f, 1.0f, 1.0f) {}

    void UpdateModelMatrix();
    void SetTexture(FTexture* InTexture) { Texture = InTexture; }
};
