// WindowLayer.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowLayer.h"
#include "bitmap.h"

#define MAX_LOADSTRING 100

#pragma comment(lib, "Msimg32.lib") // gdi
// 全局变量:
HINSTANCE hInst;                                // 当前实例
HWND parent_wnd;
HWND child_wnd;
BOOL last = TRUE;
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
DWORD WINAPI        mytimer(LPVOID args);
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
BOOL                CreatChildWindow(int nCmdShow);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ChildWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


HWND GetParentWindow(HWND child)
{
    HWND parent = GetParent(child);
    if (parent == NULL)
    {
        return child;
    }

    return GetParentWindow(parent);
}


//每1秒，3秒开始执行
DWORD WINAPI mytimer(LPVOID args)
{
    BOOL bRet = FALSE;
    MSG msg = { 0 };

    //步骤一:创建定时器
    //如果hWnd为NULL，返回值为新建立的timer的ID
    UINT timerid1 = SetTimer(NULL, 0, 1000, NULL);
    UINT timerid2 = SetTimer(NULL, 0, 3000, NULL);

    while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        if (bRet == -1)
        {
            // handle the error and possibly exit 
        }
        else
        {
            //timer消息到达
            //步骤二:定时器处理程序
            if (msg.message == WM_TIMER)
            {
                //指定的timer 到达
                if (msg.wParam == timerid1)
                {
                    RECT rect; // 矩形
                    GetClientRect(child_wnd, &rect); // 获取客户区矩形
                    HDC hdc = GetWindowDC(child_wnd); // 获取客户区 DC

                    RedrawWindow(child_wnd, &rect, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW);
                    
                    // 使用白色填充矩形
                    HBRUSH brush = CreateSolidBrush(RGB(255, 255, 128));
                    //SelectObject(hdc, brush);

                    HBITMAP cursor = GetCursorBMP();

                    HDC color_dc = CreateCompatibleDC(NULL);
                    SelectObject(color_dc, cursor);
                    
                    BLENDFUNCTION ftn = { 0 };
                    ftn.BlendOp = AC_SRC_OVER;
                    ftn.SourceConstantAlpha = 255;
                    ftn.AlphaFormat = AC_SRC_ALPHA;

                    HICON icon = GetCursorIcon();
                    
                    wchar_t out[200];

                    if (last)
                    {
                        //auto res = Rectangle(hdc, 0, 0, 300, 100);
                        //auto res = AlphaBlend(hdc, 0, 0, 32, 32, color_dc, 0, 0, 32, 32, ftn);
                        //auto res = DrawIconEx(hdc, 0, 0, icon, 32, 32, 0, NULL, DI_NORMAL);

                        //wsprintf(out, L"last is true, res = %d\n", res);
                        wsprintf(out, L"child window width=%d, height=%d\n", rect.right - rect.left, rect.bottom - rect.top);
                        OutputDebugString(out);
                        auto res = IsIconic(child_wnd);
                        wsprintf(out, L"Window minimized=%d\n", res);
                        OutputDebugString(out);
                        HWND parent = NULL;
                        parent = GetParentWindow(child_wnd);
                        res = IsIconic(parent);
                        wsprintf(out, L"Parent Window minimized=%d\n", res);
                        OutputDebugString(out);
                    }
                    else
                    {
                        //auto res = Rectangle(hdc, 300, 300, 200, 100);
                        auto res = DrawIconEx(hdc, 50, 0, icon, 32, 32, 0, NULL, DI_NORMAL);
                        res = AlphaBlend(hdc, 100, 0, 32, 32, color_dc, 0, 0, 32, 32, ftn);
                        res = DrawIcon(hdc, 0, 0, icon);

                        
                        wsprintf(out, L"last is false, res = %d\n", res);
                        OutputDebugString(out);
                    }

                    last = !last;

                    DestroyIcon(icon);
                    DeleteObject(cursor);
                    DeleteDC(color_dc);

                    DeleteObject(brush);
                    ReleaseDC(child_wnd, hdc);
                }

                //指定的timer 到达
                if (msg.wParam == timerid2)
                {
                    //
                }
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    //步骤三:定时器销毁
    KillTimer(NULL, timerid1);
    KillTimer(NULL, timerid2);

    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWLAYER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    CreatChildWindow(nCmdShow);
    CreateThread(NULL, 0, mytimer, NULL, 0, NULL);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWLAYER));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);            
        }
    }

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    HRESULT hr = S_OK;

    // Register the window class.
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(LTGRAY_BRUSH));
    wc.lpszClassName = L"DirectComposition Window Class";

    return RegisterClassEx(&wc);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   // Creates the m_hMainWindow window.
   HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW,                          // Extended window style
                              L"DirectComposition Window Class",               // Name of window class
                              L"DirectComposition Layered Child Window Sample", // Title-bar string
                              WS_MAXIMIZE | WS_SYSMENU | WS_MINIMIZEBOX,        // Top-level window
                              CW_USEDEFAULT,                                   // Horizontal position
                              CW_USEDEFAULT,                                   // Vertical position
                              1000,                                            // Width
                              700,                                             // Height
                              NULL,                                            // Parent
                              NULL,                                            // Class menu
                              GetModuleHandle(NULL),                           // Handle to application instance
                              NULL                                             // Window-creation data
   );

   if (!hWnd)
   {
       return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   parent_wnd = hWnd;

   return TRUE;
}

BOOL CreatChildWindow(int nCmdShow)
{
    HRESULT hr = S_OK;
    // Register the window class.
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    wcex.lpszClassName = L"DirectCompositionChildWindow-Child";
    RegisterClassEx(&wcex);

    // Create the playback control child window.
    child_wnd = CreateWindowEx(WS_EX_LAYERED,                           // Extended window style
                               wcex.lpszClassName,                      // Name of window class
                               NULL,                                    // Title-bar string
                               WS_CHILD | WS_CLIPSIBLINGS,              // Child window
                               0, 0, 1000, 600,                              // Window will be resized via MoveWindow
                               parent_wnd,                           // Parent
                               NULL,                                    // Class menu
                               GetModuleHandle(NULL),                   // Handle to application instance
                               NULL);                                   // Window-creation data
    
    if (!child_wnd)
    {
        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
        return FALSE;
    }

    ShowWindow(child_wnd, SW_SHOWNORMAL);
    SetLayeredWindowAttributes(child_wnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    UpdateWindow(child_wnd);
    UpdateWindow(parent_wnd);

    return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK ChildWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 分析菜单选择:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 在此处添加使用 hdc 的任何绘图代码...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
