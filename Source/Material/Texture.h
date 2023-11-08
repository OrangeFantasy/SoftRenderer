#pragma once

#include "CoreTypes.h"

#pragma warning(disable : 4819)
#include <opencv2/opencv.hpp>
#include "Texture.h"
#pragma warning(default : 4819)

struct FTexture
{
    int32 Width = 0;
    int32 Height = 0;

    uint8* Data = nullptr;

public:
    FTexture(const FString& Path);
    ~FTexture();

    FColor ReadPixel(int32 U, int32 V) const;
};

enum class ETextureSampleMode
{
    Nearest,
    Bilinear
};

class FTexureSampler
{
public:
    static FLinearColor Sample(
        const FTexture* Texture, const FVector2& UV, ETextureSampleMode TextureSampleMode = ETextureSampleMode::Nearest);
};
