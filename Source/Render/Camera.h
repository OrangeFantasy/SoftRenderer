#pragma once

#include "CoreTypes.h"

class FCamera
{
public:
    FCamera() = default;
    FCamera(const FVector& InPosition, float InFov) : Position(InPosition), Fov(InFov)
    {
        if (!InPosition.Equals(FVector::ZeroVector))
        {
            Radius = InPosition.Length();
            AzimuthAngle = FMath::Atan(InPosition.Z / (InPosition.X + SMALL_NUMBER));
            ZenithAngle = FMath::Acos(InPosition.Y / Radius);
        }
    }

    const FVector& GetCameraLocation() const { return Position; }
    float GetCameraFov() const { return Fov; }

    void SetViewRadius(float DeltaRadius)
    {
        Radius = FMath::Clamp(Radius + DeltaRadius, 5.f, 100.f);
        RecalculateCameraPosition();
    }

    void MoveCamera(float DeltaAzimuthAngle, float DeltaZenithAngle)
    {
        AzimuthAngle += DeltaAzimuthAngle;
        ZenithAngle += DeltaZenithAngle;
        RecalculateCameraPosition();
    }

private:
    FVector Position = FVector(0.f, 0.f, 10.f);
    float Fov = 60.f;

    float Radius = 10.f;
    float AzimuthAngle = 0.5 * PI;
    float ZenithAngle = 0.5 * PI;

    void RecalculateCameraPosition()
    {
        float X = Radius * FMath::Sin(ZenithAngle) * FMath::Cos(AzimuthAngle);
        float Y = Radius * FMath::Cos(ZenithAngle);
        float Z = Radius * FMath::Sin(ZenithAngle) * FMath::Sin(AzimuthAngle);
        Position = FVector(X, Y, Z);
    }
};
