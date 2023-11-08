#include "Engine/Engine.h"

#include "Render/Rasterization/RasterizationRenderer.h"
#include "Render/Window.h"
#include "Geometry/ObjParser.h"
#include "Material/Texture.h"

OEngine::OEngine()
{
    ViewportWidth = 800;
    ViewportHeight = 600;
}

OEngine::~OEngine()
{
    if (Renderer != nullptr)
    {
        delete Renderer;
        Renderer = nullptr;
    }

    if (Window != nullptr)
    {
        delete Window;
        Window = nullptr;
    }
}

bool OEngine::Initialize()
{
    Window = new FWindow(ViewportWidth, ViewportHeight, AUTO_TEXT("Soft Renderer"));
    if (Window && Window->Initialize())
    {
        FViewport Viewport(ViewportWidth, ViewportHeight, 0.1f, 50.f);
        FCamera Camera(FVector(0.f, 1.f, 10.f), 45.0f);

        Renderer = new FRasterizationRenderer();
        if (Renderer->Initialize(Viewport, Camera, EShaderType::TextureNoLightShader))
        {
            Window->SetRenderer(Renderer);
            return true;
        }
    }
    return false;
}

void OEngine::Destory() noexcept
{
    cv::destroyAllWindows();
}

int32 OEngine::Run()
{
    int32 MSAAFactor = 4;

    FMesh Mesh = FObjParser::Parse(AUTO_TEXT(R"(../../Resources/teapot_mesh.obj)"));
    FTexture Texture = FTexture(AUTO_TEXT("../../Resources/lapis_albedo.png"));
    // FMesh Mesh = FObjParser::Parse(AUTO_TEXT("../../Resources/Spot/Spot.obj"));
    // FTexture Texture = FTexture(AUTO_TEXT("../../Resources/Spot/spot_texture.png"));
    Mesh.SetTexture(&Texture);
    Mesh.SetTransform(FVector::ZeroVector, FVector(90.0f, 0.0f, 180.0f), FVector(1.5f));

    Mesh.BuildBVH();
    return 0;

    // Renderer->LoadMesh(&Mesh);
    // Renderer->SetMultiSampleAntiAliasing(false, MSAAFactor);

    // while (true)
    // {
    //     if (std::optional<int32> RunCode = FWindow::ProcessMessages())
    //     {
    //         return *RunCode;
    //     }
    //     Tick(0.1f);
    // }
}

void OEngine::Tick(float DeltaTime)
{
    Renderer->Clear();
    Renderer->Render();

    Window->Present(Renderer->GetRenderTarget());

    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 50));
}
