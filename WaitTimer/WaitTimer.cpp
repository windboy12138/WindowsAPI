// WaitTimer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <Windows.h>
#include <iostream>
#include <windef.h>
#include "..\common\common_utils.h"

int main()
{
    HANDLE timer_handle = CreateWaitableTimer(NULL, FALSE, NULL);
    if (NULL == timer_handle)
    {
        return -1;
    }

    int64_t last_time = TimeMicroseconds();
    int64_t sleep_time = 1000000;
    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -(sleep_time) * 10;
    SetWaitableTimer(timer_handle, &liDueTime, 0, NULL, NULL, 0);
    if (WaitForSingleObject(timer_handle, INFINITE) != WAIT_OBJECT_0)
    {
        //break;
    }

    int64_t cur_time = TimeMicroseconds();
    std::cout << cur_time - last_time << std::endl;
    getchar();
    return 0;
}
