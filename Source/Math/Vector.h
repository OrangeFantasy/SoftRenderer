#pragma once

#include "CoreDefines.h"
#include "Math/Math.h"

namespace Math
{
template <typename T>
struct TMatrix;

template <typename T>
struct TVector
{
    static_assert(std::is_arithmetic<T>::value, "");

public:
    union
    {
        struct
        {
            /** Vector's X component. */
            T X;

            /** Vector's Y component. */
            T Y;

            /** Vector's Z component. */
            T Z;
        };

        T XYZ[3];
    };

public:
    TVector();
    explicit TVector(T InF);
    TVector(T InX, T InY, T InZ);

    static const TVector<T> ZeroVector;

public:
    static TVector<T> Min(const TVector<T>& A, const TVector<T>& B);
    static TVector<T> Max(const TVector<T>& A, const TVector<T>& B);
    static TVector<T> Lerp(const TVector<T>& A, const TVector<T>& B, T Alpha);

    static TVector<T> CrossProduct(const TVector<T>& A, const TVector<T>& B);
    static T DotProduct(const TVector<T>& A, const TVector<T>& B);

    TVector<T> Cross(const TVector<T>& V2) const;
    T Dot(const TVector<T>& V) const;

    TVector<T> operator^(const TVector<T>& V) const;
    T operator|(const TVector<T>& V) const;

    TVector<T> operator-() const;

    TVector<T> operator+(const TVector<T>& V) const;
    TVector<T> operator-(const TVector<T>& V) const;
    TVector<T> operator*(const TVector<T>& V) const;
    TVector<T> operator/(const TVector<T>& V) const;

    TVector<T> operator+=(const TVector<T>& V);
    TVector<T> operator-=(const TVector<T>& V);
    TVector<T> operator*=(const TVector<T>& V);
    TVector<T> operator/=(const TVector<T>& V);

    template <typename FArg, typename = std::enable_if_t<std::is_arithmetic<FArg>::value>>
    TVector<T> operator+(FArg Bias) const
    {
        return TVector<T>(X + (T)Bias, Y + (T)Bias, Z + (T)Bias);
    }

    template <typename FArg, typename = std::enable_if_t<std::is_arithmetic<FArg>::value>>
    TVector<T> operator-(FArg Bias) const
    {
        return TVector<T>(X - (T)Bias, Y - (T)Bias, Z - (T)Bias);
    }

    template <typename FArg, typename = std::enable_if_t<std::is_arithmetic<FArg>::value>>
    TVector<T> operator*(FArg Scale) const
    {
        return TVector<T>(X * (T)Scale, Y * (T)Scale, Z * (T)Scale);
    }

    template <typename FArg, typename = std::enable_if_t<std::is_arithmetic<FArg>::value>>
    TVector<T> operator/(FArg Scale) const
    {
        const T RScale = T(1) / Scale;
        return TVector<T>(X * RScale, Y * RScale, Z * RScale);
    }

    template <typename FArg, typename = std::enable_if_t<std::is_arithmetic<FArg>::value>>
    TVector<T> operator*=(FArg Scale)
    {
        X *= Scale;
        Y *= Scale;
        Z *= Scale;
        return *this;
    }

    template <typename FArg, typename = std::enable_if_t<std::is_arithmetic<FArg>::value>>
    TVector<T> operator/=(FArg Scale)
    {
        const T RV = (T)1 / Scale;
        X *= RV;
        Y *= RV;
        Z *= RV;
        return *this;
    }

    T& operator[](int32 Index);
    const T& operator[](int32 Index) const;

    bool operator==(const TVector<T>& V) const;
    bool operator!=(const TVector<T>& V) const;

    bool Equals(const TVector<T>& V, T Tolerance = KINDA_SMALL_NUMBER) const;

    T Length() const;
    T SquaredLength() const;

    TVector<T> GetUnsafeNormal() const;
    TVector<T> GetSafeNormal(T Tolerance = SMALL_NUMBER) const;
};

template <typename T>
inline const TVector<T> TVector<T>::ZeroVector = TVector<T>(0.0);

template <typename T>
inline TVector<T>::TVector() : X(0), Y(0), Z(0)
{
}

template <typename T>
inline TVector<T>::TVector(T InF) : X(InF), Y(InF), Z(InF)
{
}

template <typename T>
inline TVector<T>::TVector(T InX, T InY, T InZ) : X(InX), Y(InY), Z(InZ)
{
}

template <typename T>
inline TVector<T> TVector<T>::Min(const TVector<T>& A, const TVector<T>& B)
{
    return TVector<T>(FMath::Min(A.X, B.X), FMath::Min(A.Y, B.Y), FMath::Min(A.Z, B.Z));
}

template <typename T>
inline TVector<T> TVector<T>::Max(const TVector<T>& A, const TVector<T>& B)
{
    return TVector<T>(FMath::Max(A.X, B.X), FMath::Max(A.Y, B.Y), FMath::Max(A.Z, B.Z));
}

template <typename T>
inline TVector<T> TVector<T>::Lerp(const TVector<T>& A, const TVector<T>& B, T Alpha)
{
    return Alpha * A + (1 - Alpha) * B;
}

template <typename T>
inline TVector<T> TVector<T>::CrossProduct(const TVector<T>& A, const TVector<T>& B)
{
    return A ^ B;
}

template <typename T>
inline T TVector<T>::DotProduct(const TVector<T>& A, const TVector<T>& B)
{
    return A | B;
}

template <typename T>
inline TVector<T> TVector<T>::Cross(const TVector<T>& V2) const
{
    return *this ^ V2;
}

template <typename T>
inline T TVector<T>::Dot(const TVector<T>& V) const
{
    return *this | V;
}

template <typename T>
inline TVector<T> TVector<T>::operator^(const TVector<T>& V) const
{
    return TVector<T>(Y * V.Z - Z * V.Y, Z * V.X - X * V.Z, X * V.Y - Y * V.X);
}

template <typename T>
inline T TVector<T>::operator|(const TVector<T>& V) const
{
    return X * V.X + Y * V.Y + Z * V.Z;
}

template <typename T>
inline TVector<T> TVector<T>::operator-() const
{
    return TVector<T>(-X, -Y, -Z);
}

template <typename T>
inline TVector<T> TVector<T>::operator+(const TVector<T>& V) const
{
    return TVector<T>(X + V.X, Y + V.Y, Z + V.Z);
}

template <typename T>
inline TVector<T> TVector<T>::operator-(const TVector<T>& V) const
{
    return TVector<T>(X - V.X, Y - V.Y, Z - V.Z);
}

template <typename T>
inline TVector<T> TVector<T>::operator*(const TVector<T>& V) const
{
    return TVector<T>(X * V.X, Y * V.Y, Z * V.Z);
}

template <typename T>
inline TVector<T> TVector<T>::operator/(const TVector<T>& V) const
{
    return TVector<T>(X / V.X, Y / V.Y, Z / V.Z);
}

template <typename T>
inline TVector<T> TVector<T>::operator+=(const TVector<T>& V)
{
    X += V.X;
    Y += V.Y;
    Z += V.Z;
    return *this;
}

template <typename T>
inline TVector<T> TVector<T>::operator-=(const TVector<T>& V)
{
    X -= V.X;
    Y -= V.Y;
    Z -= V.Z;
    return *this;
}

template <typename T>
inline TVector<T> TVector<T>::operator*=(const TVector<T>& V)
{
    X *= V.X;
    Y *= V.Y;
    Z *= V.Z;
    return *this;
}

template <typename T>
inline TVector<T> TVector<T>::operator/=(const TVector<T>& V)
{
    X /= V.X;
    Y /= V.Y;
    Z /= V.Z;
    return *this;
}

template <typename T>
inline T& TVector<T>::operator[](int32 Index)
{
    return XYZ[Index];
}

template <typename T>
inline const T& TVector<T>::operator[](int32 Index) const
{
    return XYZ[Index];
}

template <typename T>
inline bool TVector<T>::operator==(const TVector<T>& V) const
{
    return X == V.X && Y == V.Y && Z == V.Z;
}

template <typename T>
inline bool TVector<T>::operator!=(const TVector<T>& V) const
{
    return X != V.X || Y != V.Y || Z != V.Z;
}

template <typename T>
inline bool TVector<T>::Equals(const TVector<T>& V, T Tolerance) const
{
    return FMath::Abs(X - V.X) <= Tolerance && FMath::Abs(Y - V.Y) <= Tolerance && FMath::Abs(Z - V.Z) <= Tolerance;
}

template <typename T>
inline T TVector<T>::Length() const
{
    return FMath::Sqrt(X * X + Y * Y + Z * Z);
}

template <typename T>
inline T TVector<T>::SquaredLength() const
{
    return X * X + Y * Y + Z * Z;
}

template <typename T>
inline TVector<T> TVector<T>::GetUnsafeNormal() const
{
    const T Scale = FMath::InvSqrt(X * X + Y * Y + Z * Z);
    return TVector<T>(X * Scale, Y * Scale, Z * Scale);
}

template <typename T>
inline TVector<T> TVector<T>::GetSafeNormal(T Tolerance) const
{
    const T SquareSum = X * X + Y * Y + Z * Z;

    // Not sure if it's safe to add tolerance in there. Might introduce too many errors
    if (SquareSum == 1.f)
    {
        return *this;
    }
    else if (SquareSum < Tolerance)
    {
        return TVector<T>(0, 0, 0);
    }
    const T Scale = (T)FMath::InvSqrt(SquareSum);
    return TVector<T>(X * Scale, Y * Scale, Z * Scale);
}

} // namespace Math

template <typename T>
inline Math::TVector<T> operator*(T Scale, const Math::TVector<T>& V)
{
    return V * Scale;
}
