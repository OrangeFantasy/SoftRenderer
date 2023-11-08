#include "Geometry/BoundingBox.h"
#include "RayTracing/Ray.h"

FBoundingBox::FBoundingBox() : MinPoint(FVector(FLOAT_MAX)), MaxPoint(FVector(FLOAT_MIN)) {}

FBoundingBox::FBoundingBox(const FVector& PointA, const FVector& PointB)
    : MinPoint(FVector::Min(PointA, PointB)), MaxPoint(FVector::Max(PointA, PointB))
{
}
FBoundingBox FBoundingBox::Union(const FBoundingBox& A, const FBoundingBox& B)
{
    return A | B;
}

FBoundingBox FBoundingBox::Union(const FBoundingBox& A, const FVector& V)
{
    return A | V;
}

FBoundingBox FBoundingBox::UnionWith(const FBoundingBox& Other) const
{
    return *this | Other;
}

FBoundingBox FBoundingBox::UnionWith(const FVector& V) const
{
    return *this | V;
}

FBoundingBox FBoundingBox::operator|(const FBoundingBox& Other) const
{
    return FBoundingBox(FVector::Min(MinPoint, Other.MinPoint), FVector::Max(MaxPoint, Other.MaxPoint));
}

FBoundingBox FBoundingBox::operator|(const FVector& V) const
{
    return FBoundingBox(FVector::Min(MinPoint, V), FVector::Max(MaxPoint, V));
}

FBoundingBox& FBoundingBox::operator|=(const FBoundingBox& Other)
{
    MinPoint = FVector::Min(MinPoint, Other.MinPoint);
    MaxPoint = FVector::Max(MaxPoint, Other.MaxPoint);
    return *this;
}

FBoundingBox& FBoundingBox::operator|=(const FVector& V)
{
    MinPoint = FVector::Min(MinPoint, V);
    MaxPoint = FVector::Max(MaxPoint, V);
    return *this;
}

bool FBoundingBox::Contains(const FVector& V) const
{
    return V.X >= MinPoint.X && V.X <= MaxPoint.X && //
           V.Y >= MinPoint.Y && V.Y <= MaxPoint.Y && //
           V.Z >= MinPoint.Z && V.Z <= MaxPoint.Z;
}

FVector FBoundingBox::Centroid() const
{
    return 0.5f * (MinPoint + MaxPoint);
}

FVector FBoundingBox::Diagonal() const
{
    return MaxPoint - MinPoint;
}

int32 FBoundingBox::MaxAxis() const
{
    FVector Diag = Diagonal();
    return Diag.X > Diag.Y ? (Diag.X > Diag.Z ? 0 : 2) : (Diag.Y > Diag.Z ? 1 : 2);
}

float FBoundingBox::SurfaceArea() const
{
    FVector Diag = Diagonal();
    return 2.f * (Diag.X * Diag.Y + Diag.X * Diag.Z + Diag.Y * Diag.Z);
}

bool FBoundingBox::IsIntersecting(const FRay& Ray) const
{
    float TimeEnter = FLOAT_MIN;
    float TimeExit = FLOAT_MAX;

    for (int32 Axis = 0; Axis < 3; ++Axis)
    {
        float Tmin = (MinPoint[Axis] - Ray.Origin[Axis]) / Ray.Direction[Axis];
        float Tmax = (MaxPoint[Axis] - Ray.Origin[Axis]) / Ray.Direction[Axis];
        if (Ray.Direction[Axis] < 0.0f)
        {
            FMath::Swap(Tmin, Tmax);
        }

        TimeEnter = FMath::Max(TimeEnter, Tmin);
        TimeExit = FMath::Min(TimeExit, Tmax);
    }

    return TimeEnter <= TimeExit && TimeExit >= 0.0f;
}
