#include "Math/MathSSE.h"

#include "Math/Matrix.h"
#include "MathSSE.h"

namespace Math::SSE
{
static const uint32 stb_fp32_to_srgb8_tab4[104] = {
    0x0073000d, 0x007a000d, 0x0080000d, 0x0087000d, 0x008d000d, 0x0094000d, 0x009a000d, 0x00a1000d,
    0x00a7001a, 0x00b4001a, 0x00c1001a, 0x00ce001a, 0x00da001a, 0x00e7001a, 0x00f4001a, 0x0101001a,
    0x010e0033, 0x01280033, 0x01410033, 0x015b0033, 0x01750033, 0x018f0033, 0x01a80033, 0x01c20033,
    0x01dc0067, 0x020f0067, 0x02430067, 0x02760067, 0x02aa0067, 0x02dd0067, 0x03110067, 0x03440067,
    0x037800ce, 0x03df00ce, 0x044600ce, 0x04ad00ce, 0x051400ce, 0x057b00c5, 0x05dd00bc, 0x063b00b5,
    0x06970158, 0x07420142, 0x07e30130, 0x087b0120, 0x090b0112, 0x09940106, 0x0a1700fc, 0x0a9500f2,
    0x0b0f01cb, 0x0bf401ae, 0x0ccb0195, 0x0d950180, 0x0e56016e, 0x0f0d015e, 0x0fbc0150, 0x10630143,
    0x11070264, 0x1238023e, 0x1357021d, 0x14660201, 0x156601e9, 0x165a01d3, 0x174401c0, 0x182401af,
    0x18fe0331, 0x1a9602fe, 0x1c1502d2, 0x1d7e02ad, 0x1ed4028d, 0x201a0270, 0x21520256, 0x227d0240,
    0x239f0443, 0x25c003fe, 0x27bf03c4, 0x29a10392, 0x2b6a0367, 0x2d1d0341, 0x2ebe031f, 0x304d0300,
    0x31d105b0, 0x34a80555, 0x37520507, 0x39d504c5, 0x3c37048b, 0x3e7c0458, 0x40a8042a, 0x42bd0401,
    0x44c20798, 0x488e071e, 0x4c1c06b6, 0x4f76065d, 0x52a50610, 0x55ac05cc, 0x5892058f, 0x5b590559,
    0x5e0c0a23, 0x631c0980, 0x67db08f6, 0x6c55087f, 0x70940818, 0x74a007bd, 0x787d076c, 0x7c330723,
};

void SSE::VectorMatrixMultiply(FVector4f* Result, const FVector4f* Vector, const FMatrix4f* Matrix)
{
    const __m128* V = (const __m128*)Vector;
    const __m128* M = (const __m128*)Matrix;
    __m128* R = (__m128*)Result;
    __m128 T;

    T = _mm_mul_ps(VectorReplicate<0>(V[0]), M[0]);
    T = _mm_add_ps(_mm_mul_ps(VectorReplicate<1>(V[0]), M[1]), T);
    T = _mm_add_ps(_mm_mul_ps(VectorReplicate<2>(V[0]), M[2]), T);
    T = _mm_add_ps(_mm_mul_ps(VectorReplicate<3>(V[0]), M[3]), T);

    R[0] = T;
}

void MatrixMultiply(FMatrix4f* Result, const FMatrix4f* Matrix1, const FMatrix4f* Matrix2)
{
    const __m128* A = (const __m128*)Matrix1;
    const __m128* B = (const __m128*)Matrix2;
    __m128* R = (__m128*)Result;
    __m128 RT, R0, R1, R2;

    RT = _mm_mul_ps(VectorReplicate<0>(A[0]), B[0]);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<1>(A[0]), B[1]), RT);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<2>(A[0]), B[2]), RT);
    R0 = _mm_add_ps(_mm_mul_ps(VectorReplicate<3>(A[0]), B[3]), RT);

    RT = _mm_mul_ps(VectorReplicate<0>(A[1]), B[0]);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<1>(A[1]), B[1]), RT);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<2>(A[1]), B[2]), RT);
    R1 = _mm_add_ps(_mm_mul_ps(VectorReplicate<3>(A[1]), B[3]), RT);

    RT = _mm_mul_ps(VectorReplicate<0>(A[2]), B[0]);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<1>(A[2]), B[1]), RT);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<2>(A[2]), B[2]), RT);
    R2 = _mm_add_ps(_mm_mul_ps(VectorReplicate<3>(A[2]), B[3]), RT);

    RT = _mm_mul_ps(VectorReplicate<0>(A[3]), B[0]);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<1>(A[3]), B[1]), RT);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<2>(A[3]), B[2]), RT);
    RT = _mm_add_ps(_mm_mul_ps(VectorReplicate<3>(A[3]), B[3]), RT);

    R[0] = R0;
    R[1] = R1;
    R[2] = R2;
    R[3] = RT;
}

void SSE::MatrixInverse(FMatrix4f* DstMatrix, const FMatrix4f* SrcMatrix)
{
    using Float4x4 = float[4][4];

    const Float4x4& M = *((const Float4x4*)SrcMatrix);
    Float4x4 Result;
    float Det[4];
    Float4x4 Tmp;

    Tmp[0][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
    Tmp[0][1] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
    Tmp[0][2] = M[1][2] * M[2][3] - M[1][3] * M[2][2];

    Tmp[1][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
    Tmp[1][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
    Tmp[1][2] = M[0][2] * M[2][3] - M[0][3] * M[2][2];

    Tmp[2][0] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
    Tmp[2][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
    Tmp[2][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

    Tmp[3][0] = M[1][2] * M[2][3] - M[1][3] * M[2][2];
    Tmp[3][1] = M[0][2] * M[2][3] - M[0][3] * M[2][2];
    Tmp[3][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

    Det[0] = M[1][1] * Tmp[0][0] - M[2][1] * Tmp[0][1] + M[3][1] * Tmp[0][2];
    Det[1] = M[0][1] * Tmp[1][0] - M[2][1] * Tmp[1][1] + M[3][1] * Tmp[1][2];
    Det[2] = M[0][1] * Tmp[2][0] - M[1][1] * Tmp[2][1] + M[3][1] * Tmp[2][2];
    Det[3] = M[0][1] * Tmp[3][0] - M[1][1] * Tmp[3][1] + M[2][1] * Tmp[3][2];

    const float Determinant = M[0][0] * Det[0] - M[1][0] * Det[1] + M[2][0] * Det[2] - M[3][0] * Det[3];
    const float RDet = 1.0f / Determinant;

    Result[0][0] = +RDet * Det[0];
    Result[0][1] = -RDet * Det[1];
    Result[0][2] = +RDet * Det[2];
    Result[0][3] = -RDet * Det[3];
    Result[1][0] = -RDet * (M[1][0] * Tmp[0][0] - M[2][0] * Tmp[0][1] + M[3][0] * Tmp[0][2]);
    Result[1][1] = +RDet * (M[0][0] * Tmp[1][0] - M[2][0] * Tmp[1][1] + M[3][0] * Tmp[1][2]);
    Result[1][2] = -RDet * (M[0][0] * Tmp[2][0] - M[1][0] * Tmp[2][1] + M[3][0] * Tmp[2][2]);
    Result[1][3] = +RDet * (M[0][0] * Tmp[3][0] - M[1][0] * Tmp[3][1] + M[2][0] * Tmp[3][2]);
    Result[2][0] = +RDet * (M[1][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[2][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) +
                               M[3][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]));
    Result[2][1] = -RDet * (M[0][0] * (M[2][1] * M[3][3] - M[2][3] * M[3][1]) - M[2][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
                               M[3][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]));
    Result[2][2] = +RDet * (M[0][0] * (M[1][1] * M[3][3] - M[1][3] * M[3][1]) - M[1][0] * (M[0][1] * M[3][3] - M[0][3] * M[3][1]) +
                               M[3][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1]));
    Result[2][3] = -RDet * (M[0][0] * (M[1][1] * M[2][3] - M[1][3] * M[2][1]) - M[1][0] * (M[0][1] * M[2][3] - M[0][3] * M[2][1]) +
                               M[2][0] * (M[0][1] * M[1][3] - M[0][3] * M[1][1]));
    Result[3][0] = -RDet * (M[1][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) - M[2][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) +
                               M[3][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]));
    Result[3][1] = +RDet * (M[0][0] * (M[2][1] * M[3][2] - M[2][2] * M[3][1]) - M[2][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
                               M[3][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]));
    Result[3][2] = -RDet * (M[0][0] * (M[1][1] * M[3][2] - M[1][2] * M[3][1]) - M[1][0] * (M[0][1] * M[3][2] - M[0][2] * M[3][1]) +
                               M[3][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1]));
    Result[3][3] = +RDet * (M[0][0] * (M[1][1] * M[2][2] - M[1][2] * M[2][1]) - M[1][0] * (M[0][1] * M[2][2] - M[0][2] * M[2][1]) +
                               M[2][0] * (M[0][1] * M[1][2] - M[0][2] * M[1][1]));

    memcpy(DstMatrix, &Result, sizeof(Result));
}

int SSE::ConvertLinearToSRGB(const float* LinearColor)
{
    const __m128 RGBA = VectorLoad(LinearColor);

    // Clamp to [2^(-13), 1-eps]; these two values map to 0 and 1, respectively.
    // This clamping logic is carefully written so that NaNs map to 0.
    //
    // We do this clamping on all four color channels, even though we later handle A differently;
    // this does not change the results for A: 2^(-13) rounds to 0 in U8, 
    // and 1-eps rounds to 255 in U8, so these are OK endpoints to use.
    const __m128 AlmostOne = _mm_castsi128_ps(_mm_set1_epi32(0x3f7fffff));
    const __m128i MinValInt = _mm_set1_epi32((127 - 13) << 23);
    const __m128 MinValFlt = _mm_castsi128_ps(MinValInt);

    const __m128 Clamped = _mm_min_ps(_mm_max_ps(RGBA, MinValFlt), AlmostOne);

    // Set up for the table lookup
    // This computes a 3-vector of table indices. 
    // The above clamping ensures that the values in question are in [0,13*8-1]=[0,103].
    const __m128i TabIndexVec = _mm_srli_epi32(_mm_sub_epi32(_mm_castps_si128(Clamped), MinValInt), 20);

    // Do the 4 table lookups with regular loads. 
    // We can use PEXTRW (SSE2) to grab the 3 indices from lanes 1-3, lane 0 we can just get via MOVD.
    // The latter gives us a full 32 bits, not 16 like the other ones, but given our value range either works.
    const __m128i TabValR = _mm_cvtsi32_si128(stb_fp32_to_srgb8_tab4[(uint32)_mm_cvtsi128_si32(TabIndexVec)]);
    const __m128i TabValG = _mm_cvtsi32_si128(stb_fp32_to_srgb8_tab4[(uint32)_mm_extract_epi16(TabIndexVec, 2)]);
    const __m128i TabValB = _mm_cvtsi32_si128(stb_fp32_to_srgb8_tab4[(uint32)_mm_extract_epi16(TabIndexVec, 4)]);

    // Merge the four values we just loaded back into a 3-vector (gather complete!)
    const __m128i TabValRG = _mm_unpacklo_epi32(TabValR, TabValG);
    const __m128i TabValsRGB = _mm_unpacklo_epi64(TabValRG, TabValB); // This leaves A=0, which suits us

    // Grab the mantissa bits into the low 16 bits of each 32b lane, 
    // and set up 512 in the high 16 bits of each 32b lane, which is how the bias values in the table are meant to be scaled.
    //
    // We grab mantissa bits [12,19] for the lerp.
    const __m128i MantissaLerpFactor = _mm_and_si128(_mm_srli_epi32(_mm_castps_si128(Clamped), 12), _mm_set1_epi32(0xff));
    const __m128i FinalMultiplier = _mm_or_si128(MantissaLerpFactor, _mm_set1_epi32(512 << 16));

    // In the table:
    //    (bias >> 9) was stored in the high 16 bits
    //    scale was stored in the low 16 bits
    //    t = (mantissa >> 12) & 0xff
    //
    // then we want ((bias + scale*t) >> 16).
    // Except for the final shift, that's a single PMADDWD:
    const __m128i InterpolatedRGB = _mm_srli_epi32(_mm_madd_epi16(TabValsRGB, FinalMultiplier), 16);

    // Finally, A gets done directly, via (int)(A * 255.f + 0.5f)
    // We zero out the non-A channels by multiplying by 0; our clamping earlier took care of NaNs/infinites, so this is fine.
    const __m128 ScaledBiasedA = _mm_add_ps(_mm_mul_ps(Clamped, _mm_setr_ps(0.f, 0.f, 0.f, 255.f)), _mm_set1_ps(0.5f));
    const __m128i FinalA = _mm_cvttps_epi32(ScaledBiasedA);

    // Merge A into the result, reorder to BGRA, then pack down to bytes and store!
    // InterpolatedRGB has lane 3=0, and ComputedA has the first three lanes zero, so we can just OR them together.
    const __m128i FinalRGBA = _mm_or_si128(InterpolatedRGB, FinalA);
    const __m128i FinalBGRA = _mm_shuffle_epi32(FinalRGBA, _MM_SHUFFLE(3, 0, 1, 2));

    const __m128i Packed16 = _mm_packs_epi32(FinalBGRA, FinalBGRA);
    const __m128i Packed8 = _mm_packus_epi16(Packed16, Packed16);

    return _mm_cvtsi128_si32(Packed8);
}

} // namespace Math::SSE
