// DetectLockon.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <Wtsapi32.h>
#include <iostream>

bool IsSessionLocked()
{
    typedef BOOL(PASCAL* WTSQuerySessionInformation)(HANDLE hServer, DWORD SessionId, WTS_INFO_CLASS WTSInfoClass, LPTSTR* ppBuffer, DWORD* pBytesReturned);
    typedef void (PASCAL* WTSFreeMemory)(PVOID pMemory);

    WTSINFOEXW* pInfo = NULL;
    WTS_INFO_CLASS wtsic = WTSSessionInfoEx;
    bool bRet = false;
    LPTSTR ppBuffer = NULL;
    DWORD dwBytesReturned = 0;
    LONG dwFlags = 0;
    WTSQuerySessionInformation pWTSQuerySessionInformation = NULL;
    WTSFreeMemory pWTSFreeMemory = NULL;

    HMODULE hLib = LoadLibrary(L"wtsapi32.dll");
    if (!hLib)
    {
        return false;
    }
    pWTSQuerySessionInformation = (WTSQuerySessionInformation)GetProcAddress(hLib, "WTSQuerySessionInformationW");
    if (pWTSQuerySessionInformation)
    {
        pWTSFreeMemory = (WTSFreeMemory)GetProcAddress(hLib, "WTSFreeMemory");
        if (pWTSFreeMemory != NULL)
        {
            DWORD dwSessionID = WTSGetActiveConsoleSessionId();
            if (pWTSQuerySessionInformation(WTS_CURRENT_SERVER_HANDLE, dwSessionID, wtsic, &ppBuffer, &dwBytesReturned))
            {
                if (dwBytesReturned > 0)
                {
                    pInfo = (WTSINFOEXW*)ppBuffer;
                    if (pInfo->Level == 1)
                    {
                        dwFlags = pInfo->Data.WTSInfoExLevel1.SessionFlags;
                    }
                    if (dwFlags == WTS_SESSIONSTATE_LOCK)
                    {
                        // in Win7 and Windows Server 2008 R2, WTS_SESSIONSTATE_LOCK indicates unlock
                        bRet = true;
                    }
                }
                pWTSFreeMemory(ppBuffer);
                ppBuffer = NULL;
            }
        }
    }
    if (hLib != NULL)
    {
        FreeLibrary(hLib);
    }
    return bRet;
}

int main()
{
    HWND message_wnd_ = NULL;

    WNDCLASSEXW window_class = { 0 };
    window_class.cbSize = sizeof(window_class);
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = &DefWindowProc;
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
        0, 0, 300, 400,
        NULL, NULL, NULL, NULL);

    if (!message_wnd_)
    {
        printf_s("error code:%d! \n", GetLastError());
        return 0;
    }

    ShowWindow(message_wnd_, SW_SHOWNORMAL);


    bool is_lock = false;
    RECT rect = { 0 };
    while (true)
    {
        is_lock = IsSessionLocked();
        std::cout << "locked: " << is_lock << std::endl;
        
        GetClientRect(message_wnd_, &rect);
        std::cout << "Client size " << rect.right - rect.left << "x" << rect.bottom - rect.top << "\n";
        Sleep(1000);
    }
    
    return 0;
}
