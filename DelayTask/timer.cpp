#include <windows.h>
#include <iostream>

void CALLBACK TimerProc(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime)
{
    std::cout << "Time: " << dwTime << "ms\n";
    std::cout.flush();
}

int main1(int argc, char* argv[], char* envp[])
{
    int Counter = 0;
    MSG Msg;

    UINT TimerId = SetTimer(NULL, 0, 1000, &TimerProc); //SetTimer

    std::cout << "TimerId: " << TimerId << '\n';
    if (!TimerId)
        return -1;

    while (GetMessage(&Msg, NULL, 0, 0))
    {
        ++Counter;
        if (Msg.message == WM_TIMER)
            std::cout << "Counter: " << Counter << "; timer message\n";
        else
            std::cout << "Counter: " << Counter << "; message: " << Msg.message << '\n';
        DispatchMessage(&Msg);
    }

    KillTimer(NULL, TimerId);
    return 0;
}