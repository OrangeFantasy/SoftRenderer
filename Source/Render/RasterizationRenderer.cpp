#include "Render/RasterizationRenderer.h"

#include "Render/Primitive.h"
#include "Geometry/Triangle.h"
#include "Geometry/Mesh.h"

FRasterizationRenderer::~FRasterizationRenderer()
{
    if (Shader != nullptr)
    {
        delete Shader;
        Shader = nullptr;
    }
}

bool FRasterizationRenderer::Initialize(const FViewport& InViewport, const FCamera& InCamera, EShaderType InShaderType)
{
    switch (InShaderType)
    {
    case EShaderType::TextureShader:
        Shader = new FTextureMapShader();
        break;
    case EShaderType::TextureNoLightShader:
        Shader = new FTextureNoLightShader();
        break;
    default:
        break;
    }

    if (Shader != nullptr)
    {
        Camera = InCamera;

        Viewport = InViewport;
        ComputeViewportMatrix();

        FrameBuffer.resize((std::size_t)(Viewport.Width * Viewport.Height));
        DepthBuffer.resize((std::size_t)(Viewport.Width * Viewport.Height));

        bMSAA = false;
        MSAAFactor = 0;

        return true;
    }
    return false;
}

void FRasterizationRenderer::SetMultiSampleAntiAliasing(bool bEnable, int32 Factors)
{
    bMSAA = bEnable;
    MSAAFactor = Factors;

    if (bMSAA && MSAAFactor != 0)
    {
        MSAADepthBuffer.resize((std::size_t)(Viewport.Width * Viewport.Height * MSAAFactor));

        int32 OffsetNum = (int32)(FMath::Sqrt((float)MSAAFactor));
        float OffsetDelta = 1.0f / (float)OffsetNum;
        for (int32 X = 0; X < OffsetNum; ++X)
        {
            for (int32 Y = 0; Y < OffsetNum; ++Y)
            {
                float XOffset = (float)X * OffsetDelta + OffsetDelta / 2.0f;
                float YOffset = (float)Y * OffsetDelta + OffsetDelta / 2.0f;
                MultiSampleOffsets.emplace_back(FVector2(XOffset, YOffset));
            }
        }
    }
}

void FRasterizationRenderer::LoadMesh(FMesh* InMesh)
{
    Meshes.push_back(InMesh);
}

void FRasterizationRenderer::ComputeViewportMatrix()
{
    const FMatrix4 ViewportMatrix = FMatrix4(                                                                 //
        0.5f * Viewport.Width, 0, 0, 0.5 * Viewport.Width,                                                    //
        0, 0.5 * Viewport.Height, 0, 0.5 * Viewport.Height,                                                   //
        0, 0, (Viewport.FarPlane - Viewport.NearPlane) / 2.f, (Viewport.FarPlane + Viewport.NearPlane) / 2.f, //
        0, 0, 0, 1                                                                                            //
    );
    Shader->UploadViewportMatrix(ViewportMatrix);
}

void FRasterizationRenderer::UpdateViewMatrix()
{
    const FVector& CameraPosition = Camera.GetCameraPosition();

    ViewMatrix = FMatrix4(          //
        1, 0, 0, -CameraPosition.X, //
        0, 1, 0, -CameraPosition.Y, //
        0, 0, 1, -CameraPosition.Z, //
        0, 0, 0, 1                  //
    );
}

void FRasterizationRenderer::UpdateProjectionMatrix()
{
    float EyeFov = FMath::DegreesToRadians(Camera.GetCameraFov() / 2.0f);
    float AspectRatio = (float)Viewport.Width / Viewport.Height;

    const float& ZNear = Viewport.NearPlane;
    const float& ZFar = Viewport.FarPlane;

    ProjectionMatrix = FMatrix4(                                                  //
        1 / (AspectRatio * FMath::Tan(EyeFov)), 0, 0, 0,                          //
        0, 1 / FMath::Tan(EyeFov), 0, 0,                                          //
        0, 0, (ZNear + ZFar) / (ZNear - ZFar), 2 * ZNear * ZFar / (ZNear - ZFar), //
        0, 0, -1, 0                                                               //
    );
}

void FRasterizationRenderer::Clear()
{
    std::fill(FrameBuffer.begin(), FrameBuffer.end(), BackgroundColor);
    std::fill(DepthBuffer.begin(), DepthBuffer.end(), std::numeric_limits<float>::infinity());

    if (bMSAA && MSAAFactor != 0)
    {
        std::fill(MSAADepthBuffer.begin(), MSAADepthBuffer.end(), std::numeric_limits<float>::infinity());
    }
}

void FRasterizationRenderer::Render()
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
    Shader->UploadViewPorjectionMatrix(ViewMatrix, ProjectionMatrix);

    for (FMesh* Mesh : Meshes)
    {
        Mesh->UpdateModelMatrix();
        Shader->UploadModelMatrix(Mesh->ModelMatrix);

        RenderInternal(Mesh);
    }
}

void FRasterizationRenderer::RenderInternal(const FMesh* Mesh)
{
    const TArray<FVertex>& Vertices = Mesh->Vertices;
    const TArray<FVector3i>& Indices = Mesh->Indices;

    for (const FVector3i& TriangleIndex : Indices)
    {
        FTrianglePrimitive TrianglePrimitive;

        for (int32 i = 0; i < 3; ++i)
        {
            FVertex Vex = Vertices[TriangleIndex.XYZ[i]];

            FVertexPrimitive VertexPrimitive;
            VertexPrimitive.Position = Vex.Position;
            VertexPrimitive.Normal = Vex.Normal;
            VertexPrimitive.TexCoord = Vex.TexCoord;
            Shader->VertexShader(VertexPrimitive.VS_Position, VertexPrimitive.Position, VertexPrimitive.Normal);

            TrianglePrimitive.Vertices[i] = VertexPrimitive;
        }

        switch (RasterizationMode)
        {
        case ERasterizationMode::Triangle:
            RenderTriangle(TrianglePrimitive, Mesh->Texture);
            break;
        case ERasterizationMode::Line:
            // RenderWireframe(Triangle);
            break;
        default:
            break;
        }
    }
}

void FRasterizationRenderer::RenderTriangle(const FTrianglePrimitive& TrianglePrimitive, const FTexture* Texture)
{
    const FVertexPrimitive& VexA = TrianglePrimitive.A;
    const FVertexPrimitive& VexB = TrianglePrimitive.B;
    const FVertexPrimitive& VexC = TrianglePrimitive.C;

    int32 MinX = FMath::Clamp((int32)FMath::Floor(FMath::Min(VexA.Position.X, VexB.Position.X, VexC.Position.X)), 0, Viewport.Width);
    int32 MaxX = FMath::Clamp((int32)FMath::Ceil(FMath::Max(VexA.Position.X, VexB.Position.X, VexC.Position.X)), 0, Viewport.Width);
    int32 MinY = FMath::Clamp((int32)FMath::Floor(FMath::Min(VexA.Position.Y, VexB.Position.Y, VexC.Position.Y)), 0, Viewport.Height);
    int32 MaxY = FMath::Clamp((int32)FMath::Ceil(FMath::Max(VexA.Position.Y, VexB.Position.Y, VexC.Position.Y)), 0, Viewport.Height);

    for (int32 X = MinX; X < MaxX; ++X)
    {
        for (int32 Y = MinY; Y < MaxY; ++Y)
        {
            int32 PixelIndex = GetPixelIndex(X, Y);

            if (bMSAA)
            {
                int32 ShadePoints = 0;
                for (int32 OffsetIndex = 0; OffsetIndex < MSAAFactor; ++OffsetIndex)
                {
                    float SampleX = X + MultiSampleOffsets[OffsetIndex].X;
                    float SampleY = Y + MultiSampleOffsets[OffsetIndex].Y;

                    if (FTrianglePrimitive::PointInsideTriangle(SampleX, SampleY, TrianglePrimitive))
                    {
                        float U, V, W;
                        FTrianglePrimitive::GetTriangleBarycentric2D(SampleX, SampleY, TrianglePrimitive, U, V, W);
                        float Reciprocal = 1.0f / (U + V + W);

                        float InterpolatedZ = (U * VexA.Position.Z + V * VexB.Position.Z + W * VexC.Position.Z) * Reciprocal;
                        if (InterpolatedZ < MSAADepthBuffer[PixelIndex * MSAAFactor + OffsetIndex])
                        {
                            MSAADepthBuffer[PixelIndex * MSAAFactor + OffsetIndex] = InterpolatedZ;
                            ++ShadePoints;
                        }
                    }
                }

                if (ShadePoints > 0)
                {
                    float U, V, W;
                    FTrianglePrimitive::GetTriangleBarycentric2D(X + 0.5f, Y + 0.5f, TrianglePrimitive, U, V, W);
                    float Reciprocal = 1.0f / (U + V + W);

                    const FVector Normal = (U * VexA.Normal + V * VexB.Normal + W * VexC.Normal) * Reciprocal;
                    const FVector2 TexCoord = (U * VexA.TexCoord + V * VexB.TexCoord + W * VexC.TexCoord) * Reciprocal;
                    const FVector VS_Position = (U * VexA.VS_Position + V * VexB.VS_Position + W * VexC.VS_Position) * Reciprocal;

                    FLinearColor PixelColor;
                    Shader->PixelShader(PixelColor, VS_Position, Normal, TexCoord, Texture);

                    float CurrentColorRatio = (float)ShadePoints / MSAAFactor;
                    PixelColor = CurrentColorRatio * PixelColor + (1 - CurrentColorRatio) * FrameBuffer[PixelIndex];

                    FrameBuffer[PixelIndex] = PixelColor.ToFColorSRGB();
                }
            }
            else
            {
                float SampleX = X + 0.5f;
                float SampleY = Y + 0.5f;

                if (FTrianglePrimitive::PointInsideTriangle(SampleX, SampleY, TrianglePrimitive))
                {
                    float U, V, W;
                    FTrianglePrimitive::GetTriangleBarycentric2D(SampleX, SampleY, TrianglePrimitive, U, V, W);
                    float Reciprocal = 1.0f / (U + V + W);

                    float InterpolatedZ = (U * VexA.Position.Z + V * VexB.Position.Z + W * VexC.Position.Z) * Reciprocal;
                    if (InterpolatedZ < DepthBuffer[PixelIndex])
                    {
                        DepthBuffer[PixelIndex] = InterpolatedZ;

                        const FVector Normal = (U * VexA.Normal + V * VexB.Normal + W * VexC.Normal) * Reciprocal;
                        const FVector2 TexCoord = (U * VexA.TexCoord + V * VexB.TexCoord + W * VexC.TexCoord) * Reciprocal;
                        const FVector VS_Position = (U * VexA.VS_Position + V * VexB.VS_Position + W * VexC.VS_Position) * Reciprocal;

                        FLinearColor PixelColor;
                        Shader->PixelShader(PixelColor, VS_Position, Normal, TexCoord, Texture);

                        FrameBuffer[PixelIndex] = PixelColor.ToFColorSRGB();
                    }
                }
            }
        }
    }
}

void FRasterizationRenderer::RenderWireframe(const FTriangle& Triangle)
{
    DrawLine(Triangle.A, Triangle.B);
    DrawLine(Triangle.B, Triangle.C);
    DrawLine(Triangle.C, Triangle.A);
}

void FRasterizationRenderer::DrawLine(const FVertex& Start, const FVertex& End)
{
    int32 X1 = Start.Position.X, X2 = End.Position.X;
    int32 Y1 = Start.Position.Y, Y2 = End.Position.Y;

    int32 DX = FMath::Abs(X2 - X1), DY = FMath::Abs(Y2 - Y1);
    int32 StepX = X1 < X2 ? 1 : -1;
    int32 StepY = Y1 < Y2 ? 1 : -1;

    int32 Error = DX - DY;

    while ((X1 != X2) || (Y1 != Y2))
    {
        int32 Error2 = Error * 2;

        if (Error2 > -DY)
        {
            Error -= DY;
            X1 += StepX;
        }

        if (Error2 < DX)
        {
            Error += DX;
            Y1 += StepY;
        }

        FrameBuffer[GetPixelIndex(X1, Y1)] = FColor::Green;
    }
}

int32 FRasterizationRenderer::GetPixelIndex(int32 X, int32 Y)
{
    return (Viewport.Height - 1 - Y) * Viewport.Width + X;
}

void FRasterizationRenderer::ResetViewportSize(int32 InWidth, int32 InHeight)
{
    Viewport.Width = InWidth;
    Viewport.Height = InHeight;

    FrameBuffer.resize((std::size_t)(InWidth * InHeight));
    DepthBuffer.resize((std::size_t)(InWidth * InHeight));
    if (bMSAA && MSAAFactor != 0)
    {
        MSAADepthBuffer.resize((std::size_t)(InWidth * InHeight * MSAAFactor));
    }

    ComputeViewportMatrix();
}

void FRasterizationRenderer::SetCameraViewRadius(float DeltaRadius)
{
    Camera.SetViewRadius(DeltaRadius);
}
void FRasterizationRenderer::MoveCameraView(float DeltaAzimuthAngle, float DeltaZenithAngle)
{
    Camera.MoveCamera(DeltaAzimuthAngle, DeltaZenithAngle);
}
