#include "Geometry/Vertex.h"

FVertex::FVertex() : Position(0.f), Normal(0.f), TexCoord(0.f) {}

FVertex::FVertex(const FVector& InPosition) : Position(InPosition), Normal(0.f), TexCoord(0.f)
{
}

FVertex::FVertex(const FVector& InPosition, const FVector& InNormal)
    : Position(InPosition), Normal(InNormal), TexCoord(0.f)
{
}

FVertex::FVertex(const FVector& InPosition, const FVector& InNormal, const FVector2& InTexCoord)
    : Position(InPosition), Normal(InNormal), TexCoord(InTexCoord)
{
}

