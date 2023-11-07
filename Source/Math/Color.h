#pragma once

#include "CoreDefines.h"
#include "Math/Math.h"

struct FColor
{
    union
    {
        struct
        {
            uint8 R, G, B, A;
        };

        uint32 Bits;
    };

public:
    FORCEINLINE FColor() {}
    constexpr FORCEINLINE FColor(uint8 InR, uint8 InG, uint8 InB, uint8 InA = 255) : R(InR), G(InG), B(InB), A(InA) {}
    FORCEINLINE explicit FColor(uint32 InColor) { Bits = InColor; }

    FColor WithAlpha(uint8 Alpha) const { return FColor(R, G, B, Alpha); }

    static const FColor White;
    static const FColor Black;
    static const FColor Red;
    static const FColor Green;
    static const FColor Blue;
    static const FColor Yellow;
    static const FColor Cyan;
    static const FColor Magenta;
};

struct FLinearColor
{
    union
    {
        struct
        {
            float R, G, B, A;
        };

        float RGBA[4];
    };

public:
    static float sRGBToLinearTable[256];

    FORCEINLINE FLinearColor() {}
    constexpr FORCEINLINE FLinearColor(float InR, float InG, float InB, float InA = 1.0f) : R(InR), G(InG), B(InB), A(InA) {}

    // sRGB space -> Linear space.
    constexpr FORCEINLINE FLinearColor(const FColor& Color);

    FORCEINLINE FLinearColor operator+(const FLinearColor& ColorB) const
    {
        return FLinearColor(R + ColorB.R, G + ColorB.G, B + ColorB.B, A + ColorB.A);
    }
    FORCEINLINE FLinearColor& operator+=(const FLinearColor& ColorB)
    {
        R += ColorB.R;
        G += ColorB.G;
        B += ColorB.B;
        A += ColorB.A;
        return *this;
    }

    FORCEINLINE FLinearColor operator-(const FLinearColor& ColorB) const
    {
        return FLinearColor(R - ColorB.R, G - ColorB.G, B - ColorB.B, A - ColorB.A);
    }
    FORCEINLINE FLinearColor& operator-=(const FLinearColor& ColorB)
    {
        R -= ColorB.R;
        G -= ColorB.G;
        B -= ColorB.B;
        A -= ColorB.A;
        return *this;
    }

    FORCEINLINE FLinearColor operator*(const FLinearColor& ColorB) const
    {
        return FLinearColor(R * ColorB.R, G * ColorB.G, B * ColorB.B, A * ColorB.A);
    }
    FORCEINLINE FLinearColor& operator*=(const FLinearColor& ColorB)
    {
        R *= ColorB.R;
        G *= ColorB.G;
        B *= ColorB.B;
        A *= ColorB.A;
        return *this;
    }

    FORCEINLINE FLinearColor operator/(const FLinearColor& ColorB) const
    {
        return FLinearColor(R / ColorB.R, G / ColorB.G, B / ColorB.B, A / ColorB.A);
    }
    FORCEINLINE FLinearColor& operator/=(const FLinearColor& ColorB)
    {
        R /= ColorB.R;
        G /= ColorB.G;
        B /= ColorB.B;
        A /= ColorB.A;
        return *this;
    }

    FORCEINLINE FLinearColor operator*(float Scalar) const { return FLinearColor(R * Scalar, G * Scalar, B * Scalar, A * Scalar); }
    FORCEINLINE FLinearColor& operator*=(float Scalar)
    {
        R *= Scalar;
        G *= Scalar;
        B *= Scalar;
        A *= Scalar;
        return *this;
    }

    FORCEINLINE FLinearColor operator/(float Scalar) const
    {
        const float InvScalar = 1.0f / Scalar;
        return FLinearColor(R * InvScalar, G * InvScalar, B * InvScalar, A * InvScalar);
    }
    FORCEINLINE FLinearColor& operator/=(float Scalar)
    {
        const float InvScalar = 1.0f / Scalar;
        R *= InvScalar;
        G *= InvScalar;
        B *= InvScalar;
        A *= InvScalar;
        return *this;
    }

    FORCEINLINE bool operator==(const FLinearColor& ColorB) const
    {
        return R == ColorB.R && G == ColorB.G && B == ColorB.B && A == ColorB.A;
    }
    FORCEINLINE bool operator!=(const FLinearColor& Other) const { return R != Other.R || G != Other.G || B != Other.B || A != Other.A; }

    FORCEINLINE bool Equals(const FLinearColor& ColorB, float Tolerance = KINDA_SMALL_NUMBER) const;
    FORCEINLINE FLinearColor GetClamped(float InMin = 0.0f, float InMax = 1.0f) const;

    FColor ToFColorSRGB() const;
};

constexpr FORCEINLINE FLinearColor::FLinearColor(const FColor& Color)
    : R(sRGBToLinearTable[Color.R]), //
      G(sRGBToLinearTable[Color.G]), //
      B(sRGBToLinearTable[Color.B]), //
      A(static_cast<float>(Color.A) * (1.0f / 255.0f))
{
}

FORCEINLINE FLinearColor operator*(float Scalar, const FLinearColor& Color)
{
    return Color.operator*(Scalar);
}
