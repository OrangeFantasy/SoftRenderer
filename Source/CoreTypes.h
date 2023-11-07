#pragma once

#include "Containers/Container.h"
#include "Math/Vector2.h"
#include "Math/Vector.h"
#include "Math/Vector4.h"
#include "Math/Matrix.h"
#include "Math/Color.h"

using FVector = Math::TVector<float>;
using FVector2 = Math::TVector2<float>;
using FVector4 = Math::TVector4<float>;

using FVector3i = Math::TVector<int32>;

using FMatrix4 = Math::TMatrix<float>;

#include <iostream>

template <typename T>
std::ostream& operator<<(std::ostream& Os, const Math::TVector<T>& Vec)
{
    Os << "[ " << Vec.X << ", " << Vec.Y << ", " << Vec.Z << " ]";
    return Os;
}

template <typename T>
std::ostream& operator<<(std::ostream& Os, const Math::TVector4<T>& Vec)
{
    Os << "[ " << Vec.X << ", " << Vec.Y << ", " << Vec.Z << ", " << Vec.W << " ]";
    return Os;
}

template <typename T>
std::ostream& operator<<(std::ostream& Os, const Math::TMatrix<T>& Mat)
{
    Os << "[" << Mat(0, 0) << ", " << Mat(0, 1) << ", " << Mat(0, 2) << ", " << Mat(0, 3) << "\n";
    Os << " " << Mat(1, 0) << ", " << Mat(1, 1) << ", " << Mat(1, 2) << ", " << Mat(1, 3) << "\n";
    Os << " " << Mat(2, 0) << ", " << Mat(2, 1) << ", " << Mat(2, 2) << ", " << Mat(2, 3) << "\n";
    Os << " " << Mat(3, 0) << ", " << Mat(3, 1) << ", " << Mat(3, 2) << ", " << Mat(3, 3) << " ]";
    return Os;
}