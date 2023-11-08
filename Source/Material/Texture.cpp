#include "Material/Texture.h"

FTexture::FTexture(const FString& Path)
{
    cv::Mat Image = cv::imread(FStringUtils::ToAString(Path));
    if (!Image.empty())
    {
        if (Image.channels() == 4)
        {
            cv::cvtColor(Image, Image, cv::COLOR_BGRA2RGB);
        }
        else if (Image.channels() == 3)
        {
            cv::cvtColor(Image, Image, cv::COLOR_BGR2RGB);
        }
        cv::flip(Image, Image, 0);

        Width = Image.cols;
        Height = Image.rows;

        Data = new uint8[Width * Height * 3];
        memcpy(Data, Image.data, Width * Height * 3 * sizeof(uint8));
    }
}

FTexture::~FTexture()
{
    if (Data != nullptr)
    {
        delete[] Data;
        Data = nullptr;
    }
}

FColor FTexture::ReadPixel(int32 U, int32 V) const
{
    int32 PixelIndex = (V * Width + U) * 3;
    return FColor(Data[PixelIndex], Data[PixelIndex + 1], Data[PixelIndex + 2]);
}

FLinearColor FTexureSampler::Sample(const FTexture* Texture, const FVector2& UV, ETextureSampleMode TextureSampleMode)
{
    switch (TextureSampleMode)
    {
    case ETextureSampleMode::Nearest:
    {
        int32 IntU = (int32)(FMath::Clamp(UV.X, 0.f, 1.f) * (Texture->Width - 1) + 0.5f);
        int32 IntV = (int32)(FMath::Clamp(UV.Y, 0.f, 1.f) * (Texture->Height - 1) + 0.5f);

        return FLinearColor(Texture->ReadPixel(IntU, IntV));
    }
    case ETextureSampleMode::Bilinear:
    {
        float U = FMath::Clamp(UV.X, 0.f, 1.f) * Texture->Width;
        float V = FMath::Clamp(UV.Y, 0.f, 1.f) * Texture->Height;

        int32 FloorU = (int32)FMath::Floor(U), CeilU = FMath::Min((int32)FMath::Ceil(U), Texture->Width - 1);
        int32 FloorV = (int32)FMath::Floor(V), CeilV = FMath::Min((int32)FMath::Ceil(V), Texture->Height - 1);

        FLinearColor LD = FLinearColor(Texture->ReadPixel(FloorU, FloorV));
        FLinearColor LU = FLinearColor(Texture->ReadPixel(FloorU, CeilV));
        FLinearColor RD = FLinearColor(Texture->ReadPixel(CeilU, FloorV));
        FLinearColor RU = FLinearColor(Texture->ReadPixel(CeilU, CeilV));

        float LerpU = (U - FloorU);
        float LerpV = (V - FloorV);

        FLinearColor InterpolatedD = LerpU * LD + (1 - LerpU) * RD;
        FLinearColor InterpolatedU = LerpU * LU + (1 - LerpU) * RU;
        FLinearColor InterpolatedColor = LerpV * InterpolatedD + (1 - LerpV) * InterpolatedU;

        return InterpolatedColor;
    }
    default:
        return FLinearColor(0.f, 0.f, 0.f);
    }
}
