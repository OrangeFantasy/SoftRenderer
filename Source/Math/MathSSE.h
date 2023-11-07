#pragma once

#include <xmmintrin.h>
#include <emmintrin.h>

#include "CoreDefines.h"

#define SHUFFLEMASK(A0, A1, B2, B3) ((A0) | ((A1) << 2) | ((B2) << 4) | ((B3) << 6))

namespace Math
{

template <typename T>
struct TVector4;

template <typename T>
struct TMatrix;

namespace SSE
{

using FVector4f = TVector4<float>;
using FMatrix4f = Math::TMatrix<float>;

template <int Index>
FORCEINLINE __m128 VectorReplicate(const __m128& Vec)
{
    static_assert(Index >= 0 && Index <= 3, "Invalid Index");
    return _mm_shuffle_ps(Vec, Vec, SHUFFLEMASK(Index, Index, Index, Index));
}

FORCEINLINE __m128 VectorLoad(const float* Ptr)
{
    return _mm_loadu_ps((float*)(Ptr));
}

FORCEINLINE void VectorStore(float* Ptr, const __m128& Vec)
{
    _mm_storeu_ps(Ptr, Vec);
}

void VectorMatrixMultiply(FVector4f* Result, const FVector4f* Vector, const FMatrix4f* Matrix);

void MatrixMultiply(FMatrix4f* Result, const FMatrix4f* Matrix1, const FMatrix4f* Matrix2);

void MatrixInverse(FMatrix4f* DstMatrix, const FMatrix4f* SrcMatrix);

int ConvertLinearToSRGB(const float* LinearColor);

} // namespace SSE

} // namespace Math