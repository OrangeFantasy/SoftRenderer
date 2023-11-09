#pragma once

#include "CoreTypes.h"
#include "Geometry/Mesh.h"

class FObjParser
{
public:
    static FMesh Parse(const FString& FilePath);

private:
    static void GenerateVertices(          //
        TArray<FVertex>& OutVertices,      //
        const TArray<FVector>& Positions,  //
        const TArray<FVector>& Normals,    //
        const TArray<FVector2>& TexCoords, //
        const FString& CurrLine            //
    );

    static TArray<FString> SplitVertexIndex(const FString& String, FChar Delimiter);

    template <typename T>
    static constexpr T GetVertexElement(const TArray<T>& Array, const FString& IndexString)
    {
        int32 Index = std::stoi(IndexString);
        if (Index < 0)
        {
            return Array[(int32)Array.size() + Index];
        }
        return Array[Index - 1];
    }
};