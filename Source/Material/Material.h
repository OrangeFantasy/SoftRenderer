#pragma once

#include "CoreTypes.h"

enum class EMaterialType
{
    DIFFUSE
};

struct FMaterial
{
    EMaterialType MaterialType;
    FVector Albedo;
    FVector Emission;

    FVector Kd; // Diffusion
    FVector Ks; // Specular

    float SpecularExponent;
    float IndexOfRefraction;

public:
    FMaterial();
    FMaterial(EMaterialType InMaterialType, const FVector& InAlbedo, const FVector& InEmission = FVector(0.0f),
        const FVector& InKd = FVector(0.0f), const FVector& InKs = FVector(0.0f), float InSpecularExponent = 0.0f,
        float InIndexOfRefraction = 0.0f);

    bool IsEmission() const;

    // BRDF.
    FVector Evaluate(const FVector& Wi, const FVector& Wo, const FVector& N) const;

    // Sample the direction of the outgoing ray.
    FVector Sample(const FVector& Wi, const FVector& N) const;

    // The pdf of the sampling method.
    float PDF(const FVector& Wi, const FVector& Wo, const FVector& N) const;

private:
    FVector LocalToWorld(const FVector& V, const FVector& N) const;
};
