#pragma once

#include "Geometry/Geometry.h"
#include "Geometry/Vertex.h"

class FTriangle : public FGeometry
{
public:
    FTriangle(const FVertex& InA, const FVertex& InB, const FVertex& InC);

    virtual FBoundingBox GetBoundingBox() const override;
    virtual float GetArea() const override;

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

    float Area = 0.0f;
};
