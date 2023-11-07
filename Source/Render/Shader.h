#pragma once

#include "CoreTypes.h"
#include "Render/Light.h"

struct FTexture;

enum class EShaderType
{
    TextureShader,
    TextureNoLightShader
};

class FShader
{
public:
    FShader() = default;
    virtual ~FShader() = default;

    void UploadViewPorjectionMatrix(const FMatrix4& InViewMatrix, const FMatrix4& InProjectionMatrix);
    void UploadModelMatrix(const FMatrix4& InModelMatrix);

    void UploadViewportMatrix(const FMatrix4& InViewportMatrix);

    virtual void VertexShader(FVector& OutVSPosition, FVector& InOutPosition, FVector& InOutNormal);
    virtual void PixelShader(FLinearColor& OutPixelColor, const FVector& VS_Position, const FVector& Normal, const FVector2& Texcoord,
        const FTexture* Texture) = 0;

protected:
    FMatrix4 ModelMatrix;
    FMatrix4 ViewMatrix;
    FMatrix4 ProjectionMatrix;

    FMatrix4 MVPMatrix;
    FMatrix4 MVMatrix;
    FMatrix4 InvTransposeMVMatrix;

    FMatrix4 ViewportMatrix;

    FVector EyePosition = FVector(0.f, 0.f, 5.f);
    TArray<FLight> Lights;
};

class FTextureMapShader : public FShader
{
public:
    FTextureMapShader();

    virtual void PixelShader(FLinearColor& OutPixelColor, const FVector& VS_Position, const FVector& Normal, const FVector2& Texcoord,
        const FTexture* Texture) override;
};

class FTextureNoLightShader : public FShader
{
public:
    FTextureNoLightShader();

    virtual void PixelShader(FLinearColor& OutPixelColor, const FVector& VS_Position, const FVector& Normal, const FVector2& Texcoord,
        const FTexture* Texture) override;
};
