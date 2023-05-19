// Listener.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <iostream>

HWND message_wnd_ = NULL;

LRESULT CALLBACK ListenProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DISPLAYCHANGE:
    {
        printf_s("screen resolution changed! new size is (%4d, %4d), virtual screen size is (%4d, %4d)\n", LOWORD(lParam), HIWORD(lParam),
                 GetSystemMetrics(SM_CXVIRTUALSCREEN), GetSystemMetrics(SM_CYVIRTUALSCREEN));
        break;
    }
    case WM_DPICHANGED:
    {
        printf_s("DPI changed, new=%d\n", HIWORD(wParam));
        break;
    }        
    case WM_CLIPBOARDUPDATE:
    {
        break;
    }
    case WM_DRAWCLIPBOARD:
    {
        //HWND owner = GetClipboardOwner();
        //if (owner == message_wnd_)
        //{
        //    printf_s("message wnd changed clipboard\n");
        //    break;
        //}
        //ProcessLocalClipboardChange();
        //UpdateLocalClipboard();
        break;
    }
    case WM_CHANGECBCHAIN:
    {
        //HWND hwnd_remove = (HWND)wParam;
        //HWND hwnd_next = (HWND)lParam;
        //if (hwnd_remove == hwnd_next_viewer)
        //{
        //    hwnd_next_viewer = hwnd_next;
        //}
        //else if (hwnd_next_viewer != NULL &&
        //         hwnd_next_viewer != INVALID_HANDLE_VALUE)
        //{
        //    SendNotifyMessage(hwnd_next_viewer, WM_CHANGECBCHAIN,
        //                      (WPARAM)hwnd_remove, (LPARAM)hwnd_next);
        //}
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int main()
{
    WNDCLASSEXW window_class = { 0 };
    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = ListenProc;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 0;
    window_class.hInstance = NULL;
    window_class.hIcon = NULL;
    window_class.hCursor = NULL;
    window_class.hbrBackground = NULL;
    window_class.lpszMenuName = NULL;
    window_class.lpszClassName = L"DeviceListener";
    window_class.hIconSm = NULL;
    auto res = RegisterClassExW(&window_class);

    message_wnd_ = CreateWindowExW(
        WS_EX_LAYERED, L"DeviceListener", L"DeviceListener",
        WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, NULL, NULL);

    if (!message_wnd_)
    {
        printf_s("error code:%d! \n", GetLastError());
        return 0;
    }

    ShowWindow(message_wnd_, SW_HIDE);
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2); /// 设置窗口的dpi感知，否则设置的大小不正确

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (1)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}
