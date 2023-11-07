#pragma once

#include "CoreTypes.h"

struct FVertex
{
    FVector Position;
    FVector Normal;
    FVector2 TexCoord;

public:
    FVertex();
    FVertex(const FVector& InPosition);
    FVertex(const FVector& InPosition, const FVector& InNormal);
    FVertex(const FVector& InPosition, const FVector& InNormal, const FVector2& InTexCoord);

    // FVertex operator*(const FMatrix4& Matrix) const;
    // FVertex operator*=(const FMatrix4& Matrix);
};