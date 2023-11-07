#pragma once

#include "CoreTypes.h"
#include "Geometry/Vertex.h"

struct FTriangle
{
    union
    {
        struct
        {
            /** Vector's X component. */
            FVertex A;

            /** Vector's Y component. */
            FVertex B;

            /** Vector's Z component. */
            FVertex C;
        };

        FVertex Vertices[3];
    };

public:
    FTriangle();
    FTriangle(const FVertex& InA, const FVertex& InB, const FVertex& InC);

    void SetVertex(int32 Index, const FVertex& Vertex);

    static bool PointInsideTriangle(float X, float Y, const FTriangle& Triangle);
    static void GetTriangleBarycentric2D(float X, float Y, const FTriangle& Triangle, float& OutU, float& OutV, float& OutW);
};
