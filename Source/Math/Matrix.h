#pragma once

#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/MathSSE.h"

namespace Math
{

template <typename T>
struct alignas(16) TMatrix
{
    static_assert(std::is_floating_point_v<T>, "T must be floating point");

public:
    alignas(16) T M[4][4];

    static const TMatrix Identity;

public:
    TMatrix();
    TMatrix(const TVector<T>& InX, const TVector<T>& InY, const TVector<T>& InZ, const TVector<T>& InW);
    explicit TMatrix(T M00, T M01, T M02, T M03, T M10, T M11, T M12, T M13, T M20, T M21, T M22, T M23, T M30, T M31, T M32, T M33);

    TMatrix<T> operator*(T Other) const;

    TMatrix<T> operator+(const TMatrix<T>& Other) const;
    TMatrix<T> operator*(const TMatrix<T>& Other) const;

    TVector4<T> operator*(const TVector4<T>& Vector) const;

    bool operator==(const TMatrix<T>& Other) const;
    bool operator!=(const TMatrix<T>& Other) const;

    T operator()(int32 Row, int32 Col) const;

    TMatrix<T> Transpose() const;
    T Determinant() const;
    TMatrix<T> Inverse() const;

    TMatrix<T> LookFromMatrix(const TVector<T>& EyePosition, const TVector<T>& LookDirection, const TVector<T>& UpVector);
    TMatrix<T> LookAtMatrix(const TVector<T>& EyePosition, const TVector<T>& LookAtPosition, const TVector<T>& UpVector);
};

template <typename T>
const TMatrix<T> TMatrix<T>::Identity = TMatrix<T>(TVector<T>(1, 0, 0), TVector<T>(0, 1, 0), TVector<T>(0, 0, 1), TVector<T>(0, 0, 0));

template <typename T>
inline TMatrix<T>::TMatrix()
{
}

template <typename T>
inline TMatrix<T>::TMatrix(const TVector<T>& InX, const TVector<T>& InY, const TVector<T>& InZ, const TVector<T>& InW)
{
    M[0][0] = InX.X, M[0][1] = InX.Y, M[0][2] = InX.Z, M[0][3] = 0.0f;
    M[1][0] = InY.X, M[1][1] = InY.Y, M[1][2] = InY.Z, M[1][3] = 0.0f;
    M[2][0] = InZ.X, M[2][1] = InZ.Y, M[2][2] = InZ.Z, M[2][3] = 0.0f;
    M[3][0] = InW.X, M[3][1] = InW.Y, M[3][2] = InW.Z, M[3][3] = 1.0f;
}

template <typename T>
inline TMatrix<T>::TMatrix(T M00, T M01, T M02, T M03, T M10, T M11, T M12, T M13, T M20, T M21, T M22, T M23, T M30, T M31, T M32, T M33)
{
    M[0][0] = M00, M[0][1] = M01, M[0][2] = M02, M[0][3] = M03;
    M[1][0] = M10, M[1][1] = M11, M[1][2] = M12, M[1][3] = M13;
    M[2][0] = M20, M[2][1] = M21, M[2][2] = M22, M[2][3] = M23;
    M[3][0] = M30, M[3][1] = M31, M[3][2] = M32, M[3][3] = M33;
}

template <typename T>
inline TMatrix<T> TMatrix<T>::operator*(T Other) const
{
    TMatrix<T> ResultMat;

    for (int32 X = 0; X < 4; ++X)
    {
        ResultMat.M[X][0] = M[X][0] * Other;
        ResultMat.M[X][1] = M[X][1] * Other;
        ResultMat.M[X][2] = M[X][2] * Other;
        ResultMat.M[X][3] = M[X][3] * Other;
    }

    return ResultMat;
}

template <typename T>
inline TMatrix<T> TMatrix<T>::operator+(const TMatrix<T>& Other) const
{
    TMatrix<T> ResultMat;

    for (int32 X = 0; X < 4; ++X)
    {
        ResultMat.M[X][0] = M[X][0] + Other.M[X][0];
        ResultMat.M[X][1] = M[X][1] + Other.M[X][1];
        ResultMat.M[X][2] = M[X][2] + Other.M[X][2];
        ResultMat.M[X][3] = M[X][3] + Other.M[X][3];
    }

    return ResultMat;
}

template <typename T>
inline TMatrix<T> TMatrix<T>::operator*(const TMatrix<T>& Other) const
{
    TMatrix<T> Result;
    Math::SSE::MatrixMultiply(&Result, this, &Other);
    return Result;
}

template <typename T>
inline TVector4<T> TMatrix<T>::operator*(const TVector4<T>& Vector) const
{
    T X = M[0][0] * Vector.X + M[0][1] * Vector.Y + M[0][2] * Vector.Z + M[0][3] * Vector.W;
    T Y = M[1][0] * Vector.X + M[1][1] * Vector.Y + M[1][2] * Vector.Z + M[1][3] * Vector.W;
    T Z = M[2][0] * Vector.X + M[2][1] * Vector.Y + M[2][2] * Vector.Z + M[2][3] * Vector.W;
    T W = M[3][0] * Vector.X + M[3][1] * Vector.Y + M[3][2] * Vector.Z + M[3][3] * Vector.W;

    return TVector4<T>(X, Y, Z, W);
}

template <typename T>
inline bool TMatrix<T>::operator==(const TMatrix<T>& Other) const
{
    for (int32 X = 0; X < 4; ++X)
    {
        for (int32 Y = 0; Y < 4; ++Y)
        {
            if (M[X][Y] != Other.M[X][Y])
            {
                return false;
            }
        }
    }

    return true;
}

template <typename T>
inline bool TMatrix<T>::operator!=(const TMatrix<T>& Other) const
{
    return !(*this == Other);
}

template <typename T>
inline T TMatrix<T>::operator()(int32 Row, int32 Col) const
{
    if (Row >= 0 && Row < 4 && Col >= 0 && Col < 4)
    {
        return M[Row][Col];
    }
    return 0;
}

template <typename T>
inline TMatrix<T> TMatrix<T>::Transpose() const
{
    TMatrix<T> Result;

    Result.M[0][0] = M[0][0], Result.M[0][1] = M[1][0], Result.M[0][2] = M[2][0], Result.M[0][3] = M[3][0];
    Result.M[1][0] = M[0][1], Result.M[1][1] = M[1][1], Result.M[1][2] = M[2][1], Result.M[1][3] = M[3][1];
    Result.M[2][0] = M[0][2], Result.M[2][1] = M[1][2], Result.M[2][2] = M[2][2], Result.M[2][3] = M[3][2];
    Result.M[3][0] = M[0][3], Result.M[3][1] = M[1][3], Result.M[3][2] = M[2][3], Result.M[3][3] = M[3][3];

    return Result;
}

template <typename T>
inline T TMatrix<T>::Determinant() const
{
    return M[0][0] * (M[1][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[2][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) +
                         M[3][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2])) -
           M[1][0] * (M[0][1] * (M[2][2] * M[3][3] - M[2][3] * M[3][2]) - M[2][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
                         M[3][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2])) +
           M[2][0] * (M[0][1] * (M[1][2] * M[3][3] - M[1][3] * M[3][2]) - M[1][1] * (M[0][2] * M[3][3] - M[0][3] * M[3][2]) +
                         M[3][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2])) -
           M[3][0] * (M[0][1] * (M[1][2] * M[2][3] - M[1][3] * M[2][2]) - M[1][1] * (M[0][2] * M[2][3] - M[0][3] * M[2][2]) +
                         M[2][1] * (M[0][2] * M[1][3] - M[0][3] * M[1][2]));
}

template <typename T>
inline TMatrix<T> TMatrix<T>::Inverse() const
{
    TMatrix<T> Result;
    Math::SSE::MatrixInverse(&Result, this);
    return Result;
}

template <typename T>
inline TMatrix<T> TMatrix<T>::LookFromMatrix(const TVector<T>& EyePosition, const TVector<T>& LookDirection, const TVector<T>& UpVector)
{
    const TVector<T> ZAxis = LookDirection.GetSafeNormal();
    const TVector<T> XAxis = (UpVector ^ ZAxis).GetSafeNormal();
    const TVector<T> YAxis = ZAxis ^ XAxis;

    for (int32 RowIndex = 0; RowIndex < 3; ++RowIndex)
    {
        M[RowIndex][0] = (&XAxis.X)[RowIndex];
        M[RowIndex][1] = (&YAxis.X)[RowIndex];
        M[RowIndex][2] = (&ZAxis.X)[RowIndex];
        M[RowIndex][3] = 0.0f;
    }
    M[3][0] = -EyePosition | XAxis;
    M[3][1] = -EyePosition | YAxis;
    M[3][2] = -EyePosition | ZAxis;
    M[3][3] = 1.0f;

    return *this;
}

template <typename T>
inline TMatrix<T> TMatrix<T>::LookAtMatrix(const TVector<T>& EyePosition, const TVector<T>& LookAtPosition, const TVector<T>& UpVector)
{
    return LookFromMatrix<T>(EyePosition, (LookAtPosition - EyePosition), UpVector);
}

} // namespace Math