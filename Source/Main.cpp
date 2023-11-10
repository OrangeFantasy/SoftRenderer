#ifndef SOFT_RAY_TRACING
#include "Engine/Engine.h"

int WINAPI WinMain(                   // WinMain
    _In_ HINSTANCE Instance,          // handle to current instance
    _In_opt_ HINSTANCE hPrevInstance, // handle to previous instance
    _In_ LPSTR lpCmdLine,             // command line
    _In_ int nCmdShow                 // show state
)
{
    OEngine* Engine = new OEngine();

    int32 RunCode = 0;
    try
    {
        int32 Code = 0;
        if (Engine->Initialize())
        {
            RunCode = Engine->Run();
        }
        Engine->Destory();
    }
    catch (const std::exception& Ex)
    {
        MessageBoxA(nullptr, Ex.what(), nullptr, MB_OK);
    }

    delete Engine;
    Engine = nullptr;

    return RunCode;
}
#else

#include "Render/RayTracing/RayTracingRenderer.h"
#include "Geometry/ObjParser.h"
#include "Material/Material.h"

int main()
{
    FMaterial M_Red = FMaterial(EMaterialType::MICROFACET, FVector(0.0f), FVector(0.63f, 0.065f, 0.05f));
    FMaterial M_Green = FMaterial(EMaterialType::MICROFACET, FVector(0.0f), FVector(0.14f, 0.45f, 0.091f), FVector(0.5f));
    FMaterial M_White = FMaterial(EMaterialType::MICROFACET, FVector(0.0f), FVector(0.725f, 0.71f, 0.68f), FVector(0.2f));
    FMaterial M_Floor = FMaterial(EMaterialType::MICROFACET, FVector(0.0f), FVector(0.15, 0.3f, 0.3f));
    FMaterial M_Light = FMaterial(EMaterialType::MICROFACET, FVector(1.0f, 1.0f, 0.6f), FVector(0.65f, 0.65f, 0.65f));
    FMaterial M_Light2 = FMaterial(EMaterialType::MICROFACET, FVector(0.7f, 0.7f, 1.0f), FVector(0.65f, 0.65f, 0.65f));

    FMesh Floor = FObjParser::Parse(AUTO_TEXT("../../Resources/Models/cornellbox/floor.obj"));
    Floor.SetMaterial(&M_Floor);
    FMesh ShortBox = FObjParser::Parse(AUTO_TEXT("../../Resources/Models/cornellbox/shortbox.obj"));
    ShortBox.SetMaterial(&M_White);
    FMesh TallBox = FObjParser::Parse(AUTO_TEXT("../../Resources/Models/cornellbox/tallbox.obj"));
    TallBox.SetMaterial(&M_White);
    FMesh Left = FObjParser::Parse(AUTO_TEXT("../../Resources/Models/cornellbox/left.obj"));
    Left.SetMaterial(&M_Red);
    FMesh Right = FObjParser::Parse(AUTO_TEXT("../../Resources/Models/cornellbox/right.obj"));
    Right.SetMaterial(&M_Green);

    FMesh Light = FObjParser::Parse(AUTO_TEXT("../../Resources/Models/cornellbox/light.obj"));
    Light.SetMaterial(&M_Light);
    FMesh Light2 = Light;
    Light2.SetMaterial(&M_Light2);

    Light.ApplyTransform(FVector(-180.0f, 0.0f, 0.0f));
    Light2.ApplyTransform(FVector(180.0f, 0.0f, 0.0f));

    FCamera Camera = FCamera(FVector(278, 278, -800), 40.0f);
    FRayTracingRenderer* RTRenderer = new FRayTracingRenderer(500, 500, Camera);

    RTRenderer->AddMesh(&Floor);
    RTRenderer->AddMesh(&ShortBox);
    RTRenderer->AddMesh(&TallBox);
    RTRenderer->AddMesh(&Left);
    RTRenderer->AddMesh(&Right);
    RTRenderer->AddMesh(&Light);
    RTRenderer->AddMesh(&Light2);

    RTRenderer->BuildBVH();
    RTRenderer->Render(128);

    delete RTRenderer;
    RTRenderer = nullptr;

    return 0;
}
#endif
