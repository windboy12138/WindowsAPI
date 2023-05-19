// MultiThread.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <iostream>

#define WM_SHUTDOWN_LISTENER WM_USER + 1001

HANDLE thread1 = NULL;
HANDLE thread2 = NULL;
DWORD thread1_id = 0;


DWORD WINAPI Fun1(LPVOID lpParamter)
{
    std::cout << "Create Thread1 success!\n";
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_SHUTDOWN_LISTENER)
        {
            std::cout << "Exit Thread1 success!\n";
            break;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

DWORD WINAPI Fun2(LPVOID lpParamter)
{
    std::cout << "Create Thread2 success!\n";
    Sleep(2000);
    PostThreadMessage(thread1_id, WM_SHUTDOWN_LISTENER, NULL, NULL);
    if (thread1)
    {
        WaitForSingleObject(thread1, INFINITE);
        std::cout << "Close Thread1!\n";
        CloseHandle(thread1);
    }
    return 0;
}

int main()
{
    thread1 = CreateThread(NULL, 0, Fun1, NULL, NULL, &thread1_id);
    thread2 = CreateThread(NULL, 0, Fun2, NULL, NULL, NULL);

    Sleep(5000);
    std::cout << "Hello World!\n";
    getchar();
    return 0;
}
