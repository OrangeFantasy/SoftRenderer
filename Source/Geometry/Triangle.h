#pragma once

#include "Geometry/Geometry.h"
#include "Geometry/Vertex.h"

struct FMaterial;

class FTriangle : public FGeometry
{
public:
    FTriangle(const FVertex& InA, const FVertex& InB, const FVertex& InC, FMaterial* InMaterial);

    virtual FBoundingBox GetBoundingBox() const override;
    virtual float GetArea() const override;
    virtual void LineTrace(FHitResult& OutHitResult, const FRay& Ray) override;

private:
    union
    {
        struct
        {
            FVertex A;
            FVertex B;
            FVertex C;
        };

        FVertex Vertices[3];
    };

    FVector Normal;
    float Area = 0.0f;

    FMaterial* Material = nullptr;
};
