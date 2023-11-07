#pragma once

#include "Math/Vector.h"

namespace Math
{
template <typename T>
struct TVector2
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");

public:
    union
    {
        struct
        {
            /** Vector's X component. */
            T X;

            /** Vector's Y component. */
            T Y;
        };

        T XY[2];
    };

public:
    FORCEINLINE TVector2();
    FORCEINLINE TVector2(T InF);
    FORCEINLINE TVector2(T InX, T InY);

    static const TVector2<T> ZeroVector;

    TVector2<T> operator+(const TVector2<T>& V) const;
    TVector2<T> operator-(const TVector2<T>& V) const;
    TVector2<T> operator*(const TVector2<T>& V) const;
    TVector2<T> operator/(const TVector2<T>& V) const;
};

template <typename T>
inline const TVector2<T> TVector2<T>::ZeroVector(0.0);

template <typename T>
inline TVector2<T>::TVector2() : X(0.0), Y(0.0)
{
}

template <typename T>
inline TVector2<T>::TVector2(T InF) : X(InF), Y(InF)
{
}

template <typename T>
inline TVector2<T>::TVector2(T InX, T InY) : X(InX), Y(InY)
{
}

template <typename T>
inline TVector2<T> TVector2<T>::operator+(const TVector2<T>& V) const
{
    return TVector2<T>(X + V.X, Y + V.Y);
}

template <typename T>
inline TVector2<T> TVector2<T>::operator-(const TVector2<T>& V) const
{
    return TVector2<T>(X - V.X, Y - V.Y);
}

template <typename T>
inline TVector2<T> TVector2<T>::operator*(const TVector2<T>& V) const
{
    return TVector2<T>(X * V.X, Y * V.Y);
}

template <typename T>
inline TVector2<T> TVector2<T>::operator/(const TVector2<T>& V) const
{
    return TVector2<T>(X / V.X, Y / V.Y);
}

} // namespace Math

template <typename T>
inline Math::TVector2<T> operator*(T Scale, const Math::TVector2<T>& V)
{
    return V * Scale;
}
