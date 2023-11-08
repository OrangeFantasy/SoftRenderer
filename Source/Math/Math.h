#pragma once

#include <cmath>

#include "CoreDefines.h"

#define KINDA_SMALL_NUMBER 1.e-4f
#define SMALL_NUMBER 1.e-8f

constexpr float PI = 3.1415926535897932f;
constexpr float PI_HALF = 1.5707963267948966f;
constexpr float PI_INV = 1.f / PI;

constexpr float FLOAT_MAX = FLT_MAX;
constexpr float FLOAT_MIN = FLT_MIN;

class FMath
{
public:
    static float RadiansToDegrees(float Value) { return Value * (180.f / PI); }
    static float DegreesToRadians(float Value) { return Value * (PI / 180.f); }

    static FORCEINLINE float Sin(float Value) { return sinf(Value); }
    static FORCEINLINE double Sin(double Value) { return sin(Value); }

    static FORCEINLINE float Asin(float Value) { return asinf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
    static FORCEINLINE double Asin(double Value) { return asin((Value < -1.0) ? -1.0 : ((Value < 1.0) ? Value : 1.0)); }

    static FORCEINLINE float Sinh(float Value) { return sinhf(Value); }
    static FORCEINLINE double Sinh(double Value) { return sinh(Value); }

    static FORCEINLINE float Cos(float Value) { return cosf(Value); }
    static FORCEINLINE double Cos(double Value) { return cos(Value); }

    static FORCEINLINE float Acos(float Value) { return acosf((Value < -1.f) ? -1.f : ((Value < 1.f) ? Value : 1.f)); }
    static FORCEINLINE double Acos(double Value) { return acos((Value < -1.0) ? -1.0 : ((Value < 1.0) ? Value : 1.0)); }

    static FORCEINLINE float Tan(float Value) { return tanf(Value); }
    static FORCEINLINE double Tan(double Value) { return tan(Value); }

    static FORCEINLINE float Atan(float Value) { return atanf(Value); }
    static FORCEINLINE double Atan(double Value) { return atan(Value); }

    static FORCEINLINE float Sqrt(float Value) { return sqrtf(Value); }
    static FORCEINLINE double Sqrt(double Value) { return sqrt(Value); }

    static FORCEINLINE float Pow(float A, float B) { return powf(A, B); }
    static FORCEINLINE double Pow(double A, double B) { return pow(A, B); }

    static FORCEINLINE float InvSqrt(float F) { return 1.0f / sqrtf(F); }
    static FORCEINLINE double InvSqrt(double F) { return 1.0 / sqrt(F); }

    static FORCEINLINE float Floor(float Value) { return std::floorf(Value); }
    static FORCEINLINE double Floor(double Value) { return std::floor(Value); }

    static FORCEINLINE float Ceil(float Value) { return std::ceilf(Value); }
    static FORCEINLINE double Ceil(double Value) { return std::ceil(Value); }

    template <typename T>
    static constexpr FORCEINLINE T Clamp(const T Value, const T Min, const T Max)
    {
        return (Value < Min) ? Min : ((Value > Max) ? Max : Value);
    }

    template <typename T>
    static constexpr FORCEINLINE T Abs(const T A)
    {
        return (A < (T)0) ? -A : A;
    }

    template <typename T>
    static constexpr FORCEINLINE T Sign(const T A)
    {
        return (A > (T)0) ? (T)1 : ((A < (T)0) ? (T)-1 : (T)0);
    }

    template <typename T>
    static constexpr FORCEINLINE T Max(const T A, const T B)
    {
        return (B < A) ? A : B;
    }

    template <typename T>
    static constexpr FORCEINLINE T Min(const T A, const T B)
    {
        return (A < B) ? A : B;
    }

    template <typename T, typename... Args>
    static constexpr FORCEINLINE T Max(const T First, const Args... Other)
    {
        return Max(First, Max(Other...));
    }

    template <typename T, typename... Args>
    static constexpr FORCEINLINE T Min(const T First, const Args... Other)
    {
        return Min(First, Min(Other...));
    }

    template <typename T>
    static constexpr FORCEINLINE void Swap(T& A, T& B)
    {
        T Temp = A;
        A = B;
        B = Temp;
    }

    static FORCEINLINE float RandomFloat() { return (float)rand() / RAND_MAX; }

    static FORCEINLINE float RandomFloat(float Min, float Max) { return Min + (Max - Min) * RandomFloat(); }
};
