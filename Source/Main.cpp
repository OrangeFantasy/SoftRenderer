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