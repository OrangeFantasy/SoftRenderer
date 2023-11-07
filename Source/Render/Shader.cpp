#include "Render/Shader.h"

#include "Geometry/Texture.h"

void FShader::UploadViewPorjectionMatrix(const FMatrix4& InViewMatrix, const FMatrix4& InProjectionMatrix)
{
    std::memcpy(&ViewMatrix, &InViewMatrix, 16 * sizeof(float));
    std::memcpy(&ProjectionMatrix, &InProjectionMatrix, 16 * sizeof(float));
}

void FShader::UploadModelMatrix(const FMatrix4& InModelMatrix)
{
    std::memcpy(&ModelMatrix, &InModelMatrix, 16 * sizeof(float));

    MVMatrix = ViewMatrix * ModelMatrix;
    MVPMatrix = ProjectionMatrix * MVMatrix;
    InvTransposeMVMatrix = MVMatrix.Inverse().Transpose();
}

void FShader::UploadViewportMatrix(const FMatrix4& InViewportMatrix)
{
    std::memcpy(&ViewportMatrix, &InViewportMatrix, 16 * sizeof(float));
}

void FShader::VertexShader(FVector& OutVSPosition, FVector& InOutPosition, FVector& InOutNormal)
{
    OutVSPosition = (MVMatrix * FVector4(InOutPosition, 1.0f)).ToVector3();

    FVector4 Temp = (MVPMatrix * FVector4(InOutPosition, 1.0f)).W1();
    InOutPosition = (ViewportMatrix * Temp).ToVector3();
    InOutNormal = (InvTransposeMVMatrix * FVector4(InOutNormal, 0.0f)).ToVector3().GetSafeNormal();
}

FTextureMapShader::FTextureMapShader()
{
    Lights.emplace_back(FLight(FVector{20, 20, 20}, FVector(500, 500, 500)));
    Lights.emplace_back(FLight{FVector(-20, 20, 0), FVector(500, 500, 500)});
}

void FTextureMapShader::PixelShader(
    FLinearColor& OutPixelColor, const FVector& VS_Position, const FVector& Normal, const FVector2& Texcoord, const FTexture* Texture)
{
    FLinearColor TextureColor = FTexureSampler::Sample(Texture, Texcoord, ETextureSampleMode::Bilinear);

    FVector Ka = FVector(0.005f);
    FVector Kd = FVector(TextureColor.R, TextureColor.G, TextureColor.B);
    FVector Ks = FVector(0.7937, 0.7937, 0.7937);

    FVector AmbientIntensity = FVector(10, 10, 10);

    FVector PixelColor = FVector::ZeroVector;
    for (const FLight& Light : Lights)
    {
        FVector LightDirection = Light.Position - VS_Position;
        FVector ViewDirection = EyePosition - VS_Position;

        float LightDistance = LightDirection.SquaredLength();

        LightDirection = LightDirection.GetSafeNormal();
        ViewDirection = ViewDirection.GetSafeNormal();

        FVector Half = (LightDirection * ViewDirection).GetSafeNormal();
        float LDotN = LightDirection.Dot(Normal);
        float NDotH = Normal.Dot(Half);

        FVector Ambient = Ka * AmbientIntensity;
        FVector Diffuse = FMath::Max(LDotN, 0.f) * Kd * (Light.Intensity / LightDistance);
        FVector Specular = FMath::Pow(FMath::Max(NDotH, 0.f), 150) * Ks * (Light.Intensity / LightDistance);

        PixelColor += (Ambient + Diffuse + Specular);
    }

    OutPixelColor = FLinearColor(PixelColor.X, PixelColor.Y, PixelColor.Z);
}

FTextureNoLightShader::FTextureNoLightShader()
{
    Lights.emplace_back(FLight(FVector{20, 20, 20}, FVector(500, 500, 500)));
    Lights.emplace_back(FLight{FVector(-20, 20, 0), FVector(500, 500, 500)});
}

void FTextureNoLightShader::PixelShader(
    FLinearColor& OutPixelColor, const FVector& VS_Position, const FVector& Normal, const FVector2& Texcoord, const FTexture* Texture)
{
    OutPixelColor = FTexureSampler::Sample(Texture, Texcoord, ETextureSampleMode::Bilinear);
}
