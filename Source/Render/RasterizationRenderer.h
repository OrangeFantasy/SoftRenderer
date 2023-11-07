#pragma once

#include "CoreTypes.h"
#include "Render/Shader.h"

struct FVertex;
struct FTriangle;
struct FMesh;
struct FTrianglePrimitive;

enum class ERasterizationMode
{
    Triangle,
    Line,
};

struct FViewport
{
    int32 Width = 800;
    int32 Height = 800;
    float NearPlane = 0.1f;
    float FarPlane = 100.f;

    FViewport() = default;
    FViewport(int32 InWidth, int32 InHeight, float InNearPlane, float InFarPlane)
        : Width(InWidth), Height(InHeight), NearPlane(InNearPlane), FarPlane(InFarPlane)
    {
    }
};

struct FCamera
{
    float Fov = 60.f;
    FVector Position = FVector(0.f, 0.f, 5.f);

    float Radius = 10.f;
    void SetViewRadius(float DeltaRadius)
    {
        Radius = FMath::Clamp(Radius + DeltaRadius, 5.f, 100.f);
        Position = FVector(0.f, 1.f, Radius);
    }
};

class FRasterizationRenderer
{
public:
    FRasterizationRenderer() = default;
    ~FRasterizationRenderer();

    bool Initialize(const FViewport& InViewport, const FCamera& Camera, EShaderType InShaderType);

    void SetMultiSampleAntiAliasing(bool bEnable, int32 Factors = 4);

    void LoadMesh(FMesh* InMesh);

    void Clear();
    void Render();

    const FColor* GetRenderTarget() const { return FrameBuffer.data(); };

    void ResetViewportSize(int32 InWidth, int32 InHeight);
    void SetCameraViewRadius(float DeltaRadius);

private:
    void ComputeViewportMatrix();
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    void RenderInternal(const FMesh* Mesh);
    
    void RenderTriangle(const FTrianglePrimitive& TrianglePrimitive, const struct FTexture* Texture);
    void RenderWireframe(const FTriangle& Triangle);
    void DrawLine(const FVertex& Start, const FVertex& End);

    int32 GetPixelIndex(int32 X, int32 Y);

private:
    ERasterizationMode RasterizationMode = ERasterizationMode::Triangle;

    // Shader.
    FShader* Shader = nullptr;

    // MVP matrix.
    FMatrix4 ModelMatrix;
    FMatrix4 ViewMatrix;
    FMatrix4 ProjectionMatrix;

    // Scene meshes.
    TArray<FMesh*> Meshes;

    // Render target buffer.
    FColor BackgroundColor = FColor(0.f, 0.f, 0.f, 1.0f);
    TArray<FColor> FrameBuffer;

    // Z buffer.
    TArray<float> DepthBuffer;

    // MSAA.
    bool bMSAA = false;
    int32 MSAAFactor = 0;
    TArray<float> MSAADepthBuffer;
    TArray<FVector2> MultiSampleOffsets;

    // Viewport.
    FViewport Viewport;

    // Camera.
    FCamera Camera;
};
