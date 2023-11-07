#pragma once

#include <Windows.h>
#pragma warning(disable : 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default : 4819)

#include "CoreTypes.h"

class FRasterizationRenderer;
class FWindow;

class OEngine
{
public:
    OEngine();
    ~OEngine();

    bool Initialize();
    void Destory() noexcept;

    int32 Run();
    void Tick(float DeltaTime);

private:
    FRasterizationRenderer* Renderer = nullptr;
    FWindow* Window = nullptr;

    int32 ViewportWidth;
    int32 ViewportHeight;
};