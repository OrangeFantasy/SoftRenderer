// #pragma once

// #include "CoreTypes.h"
// #include "Geometry/Vertex.h"

// struct FBoundingBox;
// struct FHitResult;
// struct FMaterial;
// struct FRay;

// class FRTPrimitive
// {
// public:
//     FRTPrimitive() = default;
//     virtual ~FRTPrimitive() = default;

//     virtual FBoundingBox GetBoundingBox() const = 0;
//     virtual float GetArea() const = 0;
//     virtual void LineTrace(FHitResult& OutHitResult, const FRay& Ray) = 0;
// };

// class FRTTrianglePrimitive : public FRTPrimitive
// {
// public:
//     FRTTrianglePrimitive(const FVertex& InA, const FVertex& InB, const FVertex& InC, FMaterial* InMaterial);

//     virtual FBoundingBox GetBoundingBox() const override;
//     virtual float GetArea() const override;
//     virtual void LineTrace(FHitResult& OutHitResult, const FRay& Ray) override;

// private:
//     union
//     {
//         struct
//         {
//             FVertex A;
//             FVertex B;
//             FVertex C;
//         };

//         FVertex Vertices[3];
//     };

//     FVector Normal;
//     float Area = 0.0f;

//     FMaterial* Material = nullptr;
// };
