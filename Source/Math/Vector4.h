#pragma once

#include "Math/Vector.h"
#include "Math/MathSSE.h"

namespace Math
{

template <typename T>
struct alignas(16) TVector4
{
    static_assert(std::is_floating_point<T>::value, "TVector4 only supports float and double types.");

public:
    union
    {
        struct
        {
            /** The vector's X-component. */
            T X;

            /** The vector's Y-component. */
            T Y;

            /** The vector's Z-component. */
            T Z;

            /** The vector's W-component. */
            T W;
        };

        T XYZW[4];
    };

public:
    explicit TVector4(T InX = 0.0f, T InY = 0.0f, T InZ = 0.0f, T InW = 1.0f);
    explicit TVector4(const TVector<T>& V, T InW = 0.0f);

public:
    static TVector4<T> Zero() { return TVector4(T(0), T(0), T(0), T(0)); }
    static TVector4<T> One() { return TVector4(T(1), T(1), T(1), T(1)); }

    TVector4<T> operator-() const;

    TVector4<T> operator+(const TVector4<T>& V) const;
    TVector4<T> operator-(const TVector4<T>& V) const;
    TVector4<T> operator*(const TVector4<T>& V) const;
    TVector4<T> operator/(const TVector4<T>& V) const;

    TVector4<T> operator+=(const TVector4<T>& V);
    TVector4<T> operator-=(const TVector4<T>& V);
    TVector4<T> operator*=(const TVector4<T>& V);
    TVector4<T> operator/=(const TVector4<T>& V);

    TVector4<T> operator*(const TMatrix<T> M) const;

    bool operator==(const TVector4<T>& V) const;
    bool operator!=(const TVector4<T>& V) const;

    T& operator[](int32 Index);
    T operator[](int32 Index) const;

    TVector<T> ToVector3();
    TVector4<T> W1();
};

template <typename T>
inline TVector4<T>::TVector4(T InX, T InY, T InZ, T InW) : X(InX), Y(InY), Z(InZ), W(InW)
{
}

template <typename T>
inline TVector4<T>::TVector4(const TVector<T>& InV, T InW) : X(InV.X), Y(InV.Y), Z(InV.Z), W(InW)
{
}

template <typename T>
inline TVector4<T> TVector4<T>::operator-() const
{
    return TVector4(-X, -Y, -Z, -W);
}

template <typename T>
inline TVector4<T> TVector4<T>::operator+(const TVector4<T>& V) const
{
    return TVector4(X + V.X, Y + V.Y, Z + V.Z, W + V.W);
}

template <typename T>
inline TVector4<T> TVector4<T>::operator-(const TVector4<T>& V) const
{
    return TVector4(X - V.X, Y - V.Y, Z - V.Z, W - V.W);
}

template <typename T>
inline TVector4<T> TVector4<T>::operator*(const TVector4<T>& V) const
{
    return TVector4(X * V.X, Y * V.Y, Z * V.Z, W * V.W);
}

template <typename T>
inline TVector4<T> TVector4<T>::operator/(const TVector4<T>& V) const
{
    return TVector4(X / V.X, Y / V.Y, Z / V.Z, W / V.W);
}

template <typename T>
inline TVector4<T> TVector4<T>::operator+=(const TVector4<T>& V)
{
    X += V.X;
    Y += V.Y;
    Z += V.Z;
    W += V.W;
    return *this;
}

template <typename T>
inline TVector4<T> TVector4<T>::operator-=(const TVector4<T>& V)
{
    X -= V.X;
    Y -= V.Y;
    Z -= V.Z;
    W -= V.W;
    return *this;
}

template <typename T>
inline TVector4<T> TVector4<T>::operator*=(const TVector4<T>& V)
{
    X *= V.X;
    Y *= V.Y;
    Z *= V.Z;
    W *= V.W;
    return *this;
}

template <typename T>
inline TVector4<T> TVector4<T>::operator/=(const TVector4<T>& V)
{
    X /= V.X;
    Y /= V.Y;
    Z /= V.Z;
    W /= V.W;
    return *this;
}

template <typename T>
inline TVector4<T> TVector4<T>::operator*(const TMatrix<T> M) const
{
    TVector4<T> Result;
    Math::SSE::VectorMatrixMultiply(&Result, this, &M);
    return Result;
}

template <typename T>
inline bool TVector4<T>::operator==(const TVector4<T>& V) const
{
    return ((X == V.X) && (Y == V.Y) && (Z == V.Z) && (W == V.W));
}

template <typename T>
inline bool TVector4<T>::operator!=(const TVector4<T>& V) const
{
    return ((X != V.X) || (Y != V.Y) || (Z != V.Z) || (W != V.W));
}

template <typename T>
inline T& TVector4<T>::operator[](int32 Index)
{
    return XYZW[Index];
}

template <typename T>
inline T TVector4<T>::operator[](int32 Index) const
{
    return XYZW[Index];
}

template <typename T>
inline TVector<T> TVector4<T>::ToVector3()
{
    if (W == 0)
    {
        return TVector<T>(X, Y, Z);
    }
    return TVector<T>(X / W, Y / W, Z / W);
}

template <typename T>
inline TVector4<T> TVector4<T>::W1()
{
    if (W == 0)
    {
        return TVector4<T>(X, Y, Z, 1.f);
    }
    return TVector4<T>(X / W, Y / W, Z / W, 1.f);
}

} // namespace Math