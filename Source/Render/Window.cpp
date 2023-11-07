#include "Render/Window.h"

#include "Render/RasterizationRenderer.h"
#include "Window.h"

// ********************
//     Window Class
// ********************

FWindow::FWindowClass FWindow::FWindowClass::UniqueClassInstance;

FWindow::FWindowClass::FWindowClass() noexcept : InstanceHandle(GetModuleHandle(nullptr))
{
    WNDCLASSEX WndClass = {};
    WndClass.cbSize = sizeof(WndClass);
    WndClass.style = CS_OWNDC;
    WndClass.lpfnWndProc = OnWindowMsgProcess;
    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hInstance = UniqueInstance();
    WndClass.hIcon = nullptr;
    WndClass.hCursor = nullptr;
    WndClass.hbrBackground = nullptr;
    WndClass.lpszMenuName = nullptr;
    WndClass.lpszClassName = WindowClassName();
    WndClass.hIconSm = nullptr;

    RegisterClassEx(&WndClass);
}

FWindow::FWindowClass::~FWindowClass()
{
    UnregisterClass(WindowClassName(), UniqueInstance());
}

const LPCTSTR FWindow::FWindowClass::WindowClassName()
{
    return ClassName;
}

HINSTANCE FWindow::FWindowClass::UniqueInstance()
{
    return UniqueClassInstance.InstanceHandle;
}

// ********************
//        Window
// ********************

FWindow::FWindow(int32 InWidth, int32 InHeight, const FChar* InTitle) : Width(InWidth), Height(InHeight), WindowTitle(InTitle) {}

FWindow::~FWindow()
{
    DestroyWindow(WindowHandle);
}

bool FWindow::Initialize()
{
    // Define window size.
    RECT WindowRect = {};
    WindowRect.left = 100;
    WindowRect.right = Width + WindowRect.left;
    WindowRect.top = 100;
    WindowRect.bottom = Height + WindowRect.top;
    if (AdjustWindowRectEx(&WindowRect, WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_SYSMENU, FALSE, 0) && FWindowClass::UniqueInstance())
    {
        // Create window & Get hWnd.
        WindowHandle = CreateWindow(FWindowClass::WindowClassName(),    //
            WindowTitle,                                                //
            WS_CAPTION | WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_SIZEBOX, //
            CW_USEDEFAULT,                                              //
            CW_USEDEFAULT,                                              //
            WindowRect.right - WindowRect.left,                         //
            WindowRect.bottom - WindowRect.top,                         //
            nullptr,                                                    //
            nullptr,                                                    //
            FWindowClass::UniqueInstance(),                             //
            this                                                        //
        );

        // Show window.
        if (WindowHandle != nullptr)
        {
            ShowWindow(WindowHandle, SW_SHOW);
            UpdateWindow(WindowHandle);
            return true;
        }
    }
    return false;
}

void FWindow::SetRenderer(FRasterizationRenderer* InRenderer) noexcept
{
    Renderer = InRenderer;
}

std::optional<int32_t> FWindow::ProcessMessages()
{
    MSG Msg;

    // While queue has messages, remove and dispatch them (but do not block on empty queue).
    while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
    {
        // Check for quit because peekmessage does not signal this via return val.
        if (Msg.message == WM_QUIT)
        {
            return (int32_t)Msg.wParam;
        }

        // TranslateMessage will post auxilliary WM_CHAR messages from key msgs.
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    // return empty optional when not quitting app.
    return std::optional<int32_t>();
}

LRESULT CALLBACK FWindow::OnWindowMsgProcess(_In_ HWND HWnd, _In_ UINT Msg, _In_ WPARAM WParam, _In_ LPARAM LParam) noexcept
{
    // Use create parameter passed in from CreateWindow() to store window class pointer.
    if (Msg == WM_NCCREATE)
    {
        // Extract ptr to window class from creation data.
        const CREATESTRUCT* CreatePtr = reinterpret_cast<CREATESTRUCT*>(LParam);
        FWindow* WndPtr = static_cast<FWindow*>(CreatePtr->lpCreateParams);

        // Set WinAPI-managed user data to store ptr to window class.
        SetWindowLongPtr(HWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(WndPtr));

        // Set message proc to normal (non-setup) handler now that setup is finished.
        SetWindowLongPtr(HWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&FWindow::HandleMsgThunk));

        // Forward message to window class handler.
        return WndPtr->HandleMsg(HWnd, Msg, WParam, LParam);
    }
    // If we get a messgae before the WM_NCCREATE message, handle with default handler.
    return DefWindowProc(HWnd, Msg, WParam, LParam);
}

LRESULT CALLBACK FWindow::HandleMsgThunk(_In_ HWND HWnd, _In_ UINT Msg, _In_ WPARAM WParam, _In_ LPARAM LParam) noexcept
{
    // Retrieve ptr to window class.
    FWindow* WindowPtr = reinterpret_cast<FWindow*>(GetWindowLongPtr(HWnd, GWLP_USERDATA));

    // Forward message to window class handler.
    return WindowPtr->HandleMsg(HWnd, Msg, WParam, LParam);
}

LRESULT FWindow::HandleMsg(HWND HWnd, UINT Msg, WPARAM WParam, LPARAM LParam) noexcept
{
    switch (Msg)
    {
    case WM_CLOSE:
    {
        PostQuitMessage(0);
        return 0;
    }
    case WM_SIZE:
    {
        Width = LOWORD(LParam);
        Height = HIWORD(LParam);
        if (Renderer && Width != 0 && Height != 0)
        {
            Renderer->ResetViewportSize(Width, Height);
        }
        break;
    }
    case WM_MOUSEWHEEL:
    {
        int32 Delta = GET_WHEEL_DELTA_WPARAM(WParam);
        Renderer->SetCameraViewRadius(-Delta / WHEEL_DELTA);
        break;
    }
    default:
        break;
    }
    return DefWindowProc(HWnd, Msg, WParam, LParam);
}

void FWindow::Present(const void* InFrameData)
{
    HBITMAP BitmapHandle = CreateBitmap(Width, Height, 1, 32, InFrameData);
    HDC hWndDc = GetDC(WindowHandle);
    HDC hMemDc = CreateCompatibleDC(hWndDc);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDc, BitmapHandle);
    BitBlt(hWndDc, 0, 0, Width, Height, hMemDc, 0, 0, SRCCOPY);

    SelectObject(hMemDc, hOldBitmap);
    DeleteObject(BitmapHandle);
    DeleteDC(hMemDc);
    ReleaseDC(WindowHandle, hWndDc);
}
