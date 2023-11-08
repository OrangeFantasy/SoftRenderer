#include "Geometry/Triangle.h"

FTriangle::FTriangle(const FVertex& InA, const FVertex& InB, const FVertex& InC) : A(InA), B(InB), C(InC)
{
    FVector E1 = B.Position - A.Position;
    FVector E2 = C.Position - A.Position;
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

// void FTriangle::SetVertex(int32 Index, const FVertex& Vertex)
// {
//     Vertices[Index] = Vertex;
// }

// bool FTriangle::PointInsideTriangle(float X, float Y, const FTriangle& Triangle)
// {
//     const FVector& A = Triangle.A.Position;
//     const FVector& B = Triangle.B.Position;
//     const FVector& C = Triangle.C.Position;

//     const FVector P = FVector(X, Y, A.Z);
//     const FVector AC = C - A;
//     const FVector CB = B - C;
//     const FVector BA = A - B;
//     const FVector AP = P - A;
//     const FVector BP = P - B;
//     const FVector CP = P - C;

//     if (AP.Cross(AC).Dot(BP.Cross(BA)) > 0.0f    //
//         && BP.Cross(BA).Dot(CP.Cross(CB)) > 0.0f //
//         && CP.Cross(CB).Dot(AP.Cross(AC)) > 0.0f)
//     {
//         return true;
//     }
//     return false;
// }

// void FTriangle::GetTriangleBarycentric2D(float X, float Y, const FTriangle& Triangle, float& OutU, float& OutV, float& OutW)
// {
//     const FVector& A = Triangle.A.Position;
//     const FVector& B = Triangle.B.Position;
//     const FVector& C = Triangle.C.Position;

//     OutU = (X * (B.Y - C.Y) + (C.X - B.X) * Y + B.X * C.Y - C.X * B.Y) //
//            / (A.X * (B.Y - C.Y) + (C.X - B.X) * A.Y + B.X * C.Y - C.X * B.Y);
//     OutV = (X * (C.Y - A.Y) + (A.X - C.X) * Y + C.X * A.Y - A.X * C.Y) //
//            / (B.X * (C.Y - A.Y) + (A.X - C.X) * B.Y + C.X * A.Y - A.X * C.Y);
//     OutW = (X * (A.Y - B.Y) + (B.X - A.X) * Y + A.X * B.Y - B.X * A.Y) //
//            / (C.X * (A.Y - B.Y) + (B.X - A.X) * C.Y + A.X * B.Y - B.X * A.Y);
// }
