#include "Geometry/Mesh.h"

void FMesh::UpdateModelMatrix()
{
    // Rotation Matrix. Y X Z
    float Yaw = FMath::DegreesToRadians(Rotation.Y);
    float Pitch = FMath::DegreesToRadians(Rotation.X);
    float Roll = FMath::DegreesToRadians(Rotation.Z);

    float C1 = FMath::Cos(Yaw), C2 = FMath::Cos(Pitch), C3 = FMath::Cos(Roll);
    float S1 = FMath::Sin(Yaw), S2 = FMath::Sin(Pitch), S3 = FMath::Sin(Roll);

    FMatrix4 RotationMatrix = FMatrix4(                             //
        C1 * C3 + S1 * S2 * S3, C3 * S1 * S2 - C1 * S3, C2 * S1, 0, //
        C2 * S3, C2 * C3, -S2, 0,                                   //
        C1 * S2 * S3 - C3 * S1, C1 * C3 * S2 + S1 * S3, C1 * C2, 0, //
        0, 0, 0, 1                                                  //
    );

    // Scale Matrix.
    FMatrix4 ScaleMatrix = FMatrix4( //
        Scale.X, 0, 0, 0,      //
        0, Scale.Y, 0, 0,      //
        0, 0, Scale.Z, 0,      //
        0, 0, 0, 1                   //
    );

    // Translation Matrix.
    FMatrix4 TranslationMatrix = FMatrix4( //
        1, 0, 0, Position.X,         //
        0, 1, 0, Position.Y,         //
        0, 0, 1, Position.Z,         //
        0, 0, 0, 1                         //
    );

    ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;
}