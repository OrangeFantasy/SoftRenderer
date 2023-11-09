#include "Material/Material.h"

FMaterial::FMaterial()
    : MaterialType(EMaterialType::DIFFUSE), Albedo(FVector::ZeroVector), Emission(FVector::ZeroVector), Kd(FVector::ZeroVector),
      Ks(FVector::ZeroVector), SpecularExponent(0.0f), IndexOfRefraction(0.0f)
{
}

FMaterial::FMaterial(EMaterialType InMaterialType, const FVector& InAlbedo, const FVector& InEmission, const FVector& InKd,
    const FVector& InKs, float InSpecularExponent, float InIndexOfRefraction)
    : MaterialType(InMaterialType), Albedo(InAlbedo), Emission(InEmission), Kd(InKd), Ks(InKs), SpecularExponent(InSpecularExponent),
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
    default:
        return FVector::ZeroVector;
    }
}

FVector FMaterial::Sample(const FVector& Wi, const FVector& N) const
{
    switch (MaterialType)
    {
    case EMaterialType::DIFFUSE:
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
        // Uniform sample. pdf = The reciprocal of the area of the hemisphere.
        return N.Dot(Wo) > 0.0f ? 0.5f * PI_INV : 0.0f;
    default:
        return 0.0f;
    }
}
