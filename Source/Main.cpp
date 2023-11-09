#define SOFT_RAY_TRACING

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
    FMaterial M_Red = FMaterial(EMaterialType::DIFFUSE, FVector(0.0f), FVector(0.0f), FVector(0.63f, 0.065f, 0.05f));
    FMaterial M_Green = FMaterial(EMaterialType::DIFFUSE, FVector(0.0f), FVector(0.0f), FVector(0.14f, 0.45f, 0.091f));
    FMaterial M_White = FMaterial(EMaterialType::DIFFUSE, FVector(0.0f), FVector(0.0f), FVector(0.725f, 0.71f, 0.68f));
    FMaterial M_Floor = FMaterial(EMaterialType::DIFFUSE, FVector(0.0f), FVector(0.0f), FVector(0.15, 0.3f, 0.3f));

    // FVector LightIntensity = 8.0f * FVector(0.747f + 0.058f, 0.747f + 0.258f, 0.747f) +  //
    //                          15.6f * FVector(0.740f + 0.287f, 0.740f + 0.160f, 0.740f) + //
    //                          18.4f * FVector(0.737f + 0.642f, 0.737f + 0.159f, 0.737f);
    FMaterial M_Light = FMaterial(EMaterialType::DIFFUSE, FVector(0.0f), FVector(0.4f), FVector(0.65f, 0.65f, 0.65f));

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

    FCamera Camera = FCamera(FVector(278, 278, -800), 40.0f);
    FRayTracingRenderer* RTRenderer = new FRayTracingRenderer(512, 512, Camera);

    RTRenderer->AddMesh(&Floor);
    RTRenderer->AddMesh(&ShortBox);
    RTRenderer->AddMesh(&TallBox);
    RTRenderer->AddMesh(&Left);
    RTRenderer->AddMesh(&Right);
    RTRenderer->AddMesh(&Light);

    RTRenderer->BuildBVH();
    RTRenderer->Render(256);

    delete RTRenderer;
    RTRenderer = nullptr;

    return 0;
}
#endif
