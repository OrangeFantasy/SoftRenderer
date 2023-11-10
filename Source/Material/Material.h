#pragma once

#include "CoreTypes.h"

enum class EMaterialType
{
    DIFFUSE,
    MICROFACET
};

class FMaterialShader;

struct FMaterial
{
    EMaterialType MaterialType;
    FVector Emission;

    FVector Kd; // Diffusion
    FVector Ks; // Specular

    float Roughness = 0.02f;

    float SpecularExponent;
    float IndexOfRefraction;

public:
    FMaterial();
    FMaterial(EMaterialType InMaterialType, const FVector& InEmission = FVector(0.0f), const FVector& InKd = FVector(0.0f),
        const FVector& InKs = FVector(0.0f), float InSpecularExponent = 0.0f, float InIndexOfRefraction = 0.0f);

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

class FMaterialShader
{
public:
    static FVector CookTorranceBRDF(const FMaterial* Material, const FVector& Wi, const FVector& Wo, const FVector& N);

private:
    static float D_TrowbridgeReitzGGX(float NDotH, float Alpha);
    static float G_SchlickSmithGGX(float NDotV, float NDotL, float Alpha);
    static float F_Schlick(float VDotH, float F0);
};
