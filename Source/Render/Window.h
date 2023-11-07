#pragma once

#include "CoreTypes.h"

#include <Windows.h>
#include <optional>

class FRasterizationRenderer;
class FMouse;

class FWindow
{
private:
    class FWindowClass
    {
    public:
        FWindowClass() noexcept;
        ~FWindowClass() noexcept;

        static const LPCTSTR WindowClassName();
        static HINSTANCE UniqueInstance();

    private:
        static constexpr LPCTSTR ClassName = AUTO_TEXT("SoftRendererWindowClass");
        static FWindowClass UniqueClassInstance;

        HINSTANCE InstanceHandle;
    };

public:
    FWindow(int32 InWidth, int32 InHeight, const FChar* InTitle);
    ~FWindow();

    bool Initialize();
    void SetRenderer(FRasterizationRenderer* InRenderer) noexcept;

    static std::optional<int32_t> ProcessMessages();
    static LRESULT CALLBACK OnWindowMsgProcess(_In_ HWND HWnd, _In_ UINT Msg, _In_ WPARAM WParam, _In_ LPARAM LParam) noexcept;

    void Present(const void* InFrameData);

private:
    static LRESULT CALLBACK HandleMsgThunk(_In_ HWND HWnd, _In_ UINT Msg, _In_ WPARAM WParam, _In_ LPARAM LParam) noexcept;
    LRESULT HandleMsg(HWND HWnd, UINT Msg, WPARAM WParam, LPARAM LParam) noexcept;

    bool CreateExternalDevices() noexcept;
    void DestroyExternalDevices() noexcept;

private:
    HWND WindowHandle;

    int32 Width;
    int32 Height;
    LPCTSTR WindowTitle;

    FRasterizationRenderer* Renderer = nullptr;
    FMouse* Mouse;
};
