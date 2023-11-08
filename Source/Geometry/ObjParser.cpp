#include "Geometry/ObjParser.h"

#include <fstream>
#include <sstream>
#include <cassert>

enum class EVertexType
{
    Position,
    PositionNormal,
    PositionTexcoord,
    PositionNormalTexcoord,
};

FMesh FObjParser::Parse(const FString& FilePath)
{
    FMesh Mesh;
    FFileStream ObjFile(FilePath);

    if (ObjFile.is_open())
    {
        TArray<FVector> Positions;
        TArray<FVector> Normals;
        TArray<FVector2> TexCoords;

        FString Line, Key, X, Y, Z;
        int32 VertexIndex = 0;
        while (!ObjFile.eof())
        {
            std::getline(ObjFile, Line);
            FStringStream LineStream(Line);
            LineStream >> Key;

            if (Key == AUTO_TEXT("v"))
            {
                LineStream >> X >> Y >> Z;
                FVector Position = FVector(std::stof(X), std::stof(Y), std::stof(Z));
                Positions.emplace_back(std::move(Position));
            }
            else if (Key == AUTO_TEXT("vn"))
            {
                LineStream >> X >> Y >> Z;
                FVector Normal = FVector(std::stof(X), std::stof(Y), std::stof(Z));
                Normals.emplace_back(std::move(Normal));
            }
            else if (Key == AUTO_TEXT("vt"))
            {
                LineStream >> X >> Y;
                FVector2 TexCoord = FVector2(std::stof(X), std::stof(Y));
                TexCoords.emplace_back(std::move(TexCoord));
            }
            else if (Key == AUTO_TEXT("f"))
            {
                TArray<FVertex> Vertices;
                GenerateVertices(Vertices, Positions, Normals, TexCoords, Line);

                for (FVertex& Vertex : Vertices)
                {
                    Mesh.AddVertex(Vertex);
                }
                Mesh.AddIndex(FVector3i(VertexIndex, VertexIndex + 1, VertexIndex + 2));

                VertexIndex += 3;
            }

            Key = AUTO_TEXT("");
        }
    }
    ObjFile.close();

    return Mesh;
}

void FObjParser::GenerateVertices(     //
    TArray<FVertex>& OutVertices,      //
    const TArray<FVector>& Positions,  //
    const TArray<FVector>& Normals,    //
    const TArray<FVector2>& TexCoords, //
    const FString& CurrLine            //
)
{
    TArray<FString> VexIndices = SplitVertexIndex(CurrLine, ' ');
    TArray<FString> SplitIndex;
    assert(VexIndices.size() == 4);

    bool bExistNormal = true;
    for (const FString& VexIndex : VexIndices) // 3 Vertices.
    {
        if (VexIndex == AUTO_TEXT("f"))
        {
            continue;
        }

        FVertex Vertex;
        EVertexType VertexType;

        SplitIndex = SplitVertexIndex(VexIndex, AUTO_TEXT('/')); // Position, Normal, Texcoord.
        switch (SplitIndex.size())
        {
        case 1:
            VertexType = EVertexType::Position;
            break;
        case 2:
            VertexType = EVertexType::PositionTexcoord;
            break;
        case 3:
            VertexType = (SplitIndex[1] == AUTO_TEXT("") ? EVertexType::PositionNormal : EVertexType::PositionNormalTexcoord);
            break;
        default:
            break;
        }

        switch (VertexType)
        {
        case EVertexType::Position:
            Vertex.Position = GetVertexElement(Positions, SplitIndex[0]);
            Vertex.TexCoord = FVector2::ZeroVector;
            bExistNormal = false;
            break;
        case EVertexType::PositionTexcoord:
            Vertex.Position = GetVertexElement(Positions, SplitIndex[0]);
            Vertex.TexCoord = GetVertexElement(TexCoords, SplitIndex[1]);
            bExistNormal = false;
            break;
        case EVertexType::PositionNormal:
            Vertex.Position = GetVertexElement(Positions, SplitIndex[0]);
            Vertex.TexCoord = FVector2::ZeroVector;
            Vertex.Normal = GetVertexElement(Normals, SplitIndex[2]);
            break;
        case EVertexType::PositionNormalTexcoord:
            Vertex.Position = GetVertexElement(Positions, SplitIndex[0]);
            Vertex.TexCoord = GetVertexElement(TexCoords, SplitIndex[1]);
            Vertex.Normal = GetVertexElement(Normals, SplitIndex[2]);
            break;
        default:
            break;
        }

        OutVertices.emplace_back(std::move(Vertex));
    }

    if (!bExistNormal)
    {
        FVector AB = OutVertices[1].Position - OutVertices[0].Position;
        FVector AC = OutVertices[2].Position - OutVertices[0].Position;

        FVector Normal = FVector::CrossProduct(AB, AC).GetSafeNormal();
        for (FVertex& Vertex : OutVertices)
        {
            Vertex.Normal = Normal;
        }
    }
}

TArray<FString> FObjParser::SplitVertexIndex(const FString& String, FChar Delimiter)
{
    FStringStream StringStream(String);
    FString Token;
    TArray<FString> Splited;

    while (std::getline(StringStream, Token, Delimiter))
    {
        Splited.emplace_back(std::move(Token));
    }
    return Splited;
}
