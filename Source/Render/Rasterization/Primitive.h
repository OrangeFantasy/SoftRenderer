#pragma once

#include "CoreTypes.h"

struct FVertexPrimitive
{
    FVector Position;
    FVector Normal;
    FVector2 TexCoord;

    FVector VS_Position;

    FVertexPrimitive(){};
};

struct FTrianglePrimitive
{
    union
    {
        struct
        {
            FVertexPrimitive A;
            FVertexPrimitive B;
            FVertexPrimitive C;
        };

        FVertexPrimitive Vertices[3];
    };

    FTrianglePrimitive() {}

    static bool PointInsideTriangle(float X, float Y, const FTrianglePrimitive& Triangle);
    static void GetTriangleBarycentric2D(float X, float Y, const FTrianglePrimitive& Triangle, float& OutU, float& OutV, float& OutW);
};
