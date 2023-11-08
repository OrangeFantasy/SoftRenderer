#include "Geometry/Triangle.h"

#include "Geometry/BoundingBox.h"
#include "RayTracing/HitResult.h"
#include "RayTracing/Ray.h"

FTriangle::FTriangle(const FVertex& InA, const FVertex& InB, const FVertex& InC, FMaterial* InMaterial)
    : A(InA), B(InB), C(InC), Material(InMaterial)
{
    FVector E1 = B.Position - A.Position;
    FVector E2 = C.Position - A.Position;
    Normal = FVector::CrossProduct(E1, E2).GetSafeNormal();
    Area = 0.5f * FVector::CrossProduct(E1, E2).Length();
}

FBoundingBox FTriangle::GetBoundingBox() const
{
    return FBoundingBox(A.Position, B.Position) | C.Position;
}

float FTriangle::GetArea() const
{
    return Area;
}

void FTriangle::LineTrace(FHitResult& OutHitResult, const FRay& Ray)
{
    if (FVector::DotProduct(Ray.Direction, Normal) < 0.0f)
    {
        // Möller–Trumbore:
        // O + t * d = (1 - u - v) * P0 + u * P1 + v * P2
        // => O - P0 = (P1 - P0) * u + (P2 - P0) * v - t * d
        // => S      = E1 * u        + E2 * v        - t * d
        //                   | t |
        // => S = [-d E1 E2] | u |
        //                   | v |
        // => t = det([S E1 E2]) / det([-d E1 E2])
        //        (S x E1) · E2
        // => t = --------------
        //        E1 · (d x E2)
        // => t = (S2 · E2) / (S1 · E1)
        //
        // u = (S1 · S) / (S1 · E1)
        // v = (S2 · d) / (S1 · E1)

        FVector E1 = B.Position - A.Position;
        FVector E2 = C.Position - A.Position;
        FVector S1 = FVector::CrossProduct(Ray.Direction, E2);
        float Det = FVector::DotProduct(S1, E1);
        if (FMath::Abs(Det) > KINDA_SMALL_NUMBER)
        {
            FVector S = Ray.Direction - A.Position;
            FVector S2 = FVector::CrossProduct(S, E1);

            float DetInv = 1.0f / Det;
            float T = DetInv * FVector::DotProduct(S2, E2);
            float U = DetInv * FVector::DotProduct(S1, S);
            float V = DetInv * FVector::DotProduct(S2, Ray.Direction);

            if (T >= 0 && U >= 0.0f && V >= 0.0f && U + V <= 1.0f)
            {
                OutHitResult.bHit = true;
                OutHitResult.Location = Ray.GetLocation(T);
                OutHitResult.Normal = Normal;
                OutHitResult.Time = T;
                OutHitResult.Object = this;
                OutHitResult.Material = Material;
            }
        }
    }
}
