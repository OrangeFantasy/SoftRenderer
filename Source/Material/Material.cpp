#include "Material/Material.h"

FMaterial::FMaterial()
    : MaterialType(EMaterialType::DIFFUSE), Emission(FVector::ZeroVector), Kd(FVector::ZeroVector), Ks(FVector::ZeroVector),
      SpecularExponent(0.0f), IndexOfRefraction(0.0f)
{
}

FMaterial::FMaterial(EMaterialType InMaterialType, const FVector& InEmission, const FVector& InKd, const FVector& InKs,
    float InSpecularExponent, float InIndexOfRefraction)
    : MaterialType(InMaterialType), Emission(InEmission), Kd(InKd), Ks(InKs), SpecularExponent(InSpecularExponent),
      IndexOfRefraction(InIndexOfRefraction)
{
}

FVector FMaterial::LocalToWorld(const FVector& V, const FVector& N) const
{
    FVector E2 = FMath::Abs(N.X) > FMath::Abs(N.Y) ? FVector(N.Z, 0.0f, -N.X) / FMath::Sqrt(N.X * N.X + N.Z * N.Z)
                                                   : FVector(0.0f, N.Z, -N.Y) / FMath::Sqrt(N.Y * N.Y + N.Z * N.Z);
    FVector E1 = FVector::CrossProduct(E2, N);
    return E1 * V.X + E2 * V.Y + N * V.Z;
}

bool FMaterial::IsEmission() const
{
    return !Emission.Equals(FVector::ZeroVector);
}

FVector FMaterial::Evaluate(const FVector& Wi, const FVector& Wo, const FVector& N) const
{
    switch (MaterialType)
    {
    case EMaterialType::DIFFUSE:
        return N.Dot(Wo) > 0.0f ? Kd * PI_INV : FVector::ZeroVector;
    case EMaterialType::MICROFACET:
        return N.Dot(Wo) > 0.0f ? FMaterialShader::CookTorranceBRDF(this, Wi, Wo, N) : FVector::ZeroVector;
    default:
        return FVector::ZeroVector;
    }
}

FVector FMaterial::Sample(const FVector& Wi, const FVector& N) const
{
    switch (MaterialType)
    {
    case EMaterialType::DIFFUSE:
    case EMaterialType::MICROFACET:
    {
        // Uniform sample on the hemisphere.
        float R1 = FMath::RandomFloat(), R2 = FMath::RandomFloat();
        float Z = FMath::Abs(1.0f - 2.0f * R1);

        float Radius = FMath::Sqrt(1.0f - Z * Z);
        float Angle = 2.0f * PI * R2;

        FVector LocalRay = FVector(Radius * FMath::Cos(Angle), Radius * FMath::Sin(Angle), Z);
        return LocalToWorld(LocalRay, N);
    }
    default:
        return FVector::ZeroVector;
    }
}

float FMaterial::PDF(const FVector& Wi, const FVector& Wo, const FVector& N) const
{
    switch (MaterialType)
    {
    case EMaterialType::DIFFUSE:
    case EMaterialType::MICROFACET:
        // Uniform sample. pdf = The reciprocal of the area of the hemisphere.
        return N.Dot(Wo) > 0.0f ? 0.5f * PI_INV : 0.0f;
    default:
        return 0.0f;
    }
}

FVector FMaterialShader::CookTorranceBRDF(const FMaterial* Material, const FVector& Wi, const FVector& Wo, const FVector& N)
{
    FVector V = -Wi, L = Wo;
    FVector H = (V + L).GetSafeNormal();

    float NDotH = FMath::Max(N.Dot(H), KINDA_SMALL_NUMBER);
    float NDotL = FMath::Max(N.Dot(L), KINDA_SMALL_NUMBER);
    float NDotV = FMath::Max(N.Dot(V), KINDA_SMALL_NUMBER);
    float VDotH = FMath::Max(L.Dot(H), KINDA_SMALL_NUMBER);

    float D = D_TrowbridgeReitzGGX(NDotH, Material->Roughness);
    float G = G_SchlickSmithGGX(NDotV, NDotL, Material->Roughness);
    float F = F_Schlick(VDotH, 0.5f);

    float Diffuse = NDotV + 0.5f;
    float Specular = D * G * F / (4.0f * NDotV * NDotL);

    return Diffuse * Material->Kd * PI_INV + Specular * Material->Ks;
}

float FMaterialShader::D_TrowbridgeReitzGGX(float NDotH, float Alpha)
{
    float Alpha2 = Alpha * Alpha;
    float NDotH2 = NDotH * NDotH;
    float D = NDotH2 * (Alpha2 - 1.0f) + 1.0f;
    return Alpha2 * PI_INV / (D * D);
}

float FMaterialShader::G_SchlickSmithGGX(float NDotV, float NDotL, float Roughness)
{
    float K = (Roughness + 1.0f) * (Roughness + 1.0f) * 0.125f;
    auto Schlick = [&](float NDot_) -> float
    {
        float Denom = NDot_ * (1.0f - K) + K;
        return NDot_ / FMath::Max(Denom, KINDA_SMALL_NUMBER);
    };
    return Schlick(NDotV) * Schlick(NDotL);
}

float FMaterialShader::F_Schlick(float VDotH, float F0)
{
    return F0 + (1 - F0) * FMath::Pow(2.0f, (-5.55473f * VDotH - 6.98316f) * VDotH);
}
