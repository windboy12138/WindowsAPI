// LittleBigEndian.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include <winsock.h>

#pragma comment (lib, "Ws2_32.lib")

typedef unsigned short CARD16;
typedef unsigned int CARD32;

#define Swap16IfLE(s) \
    ((CARD16) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff)))
#define Swap32IfLE(l) \
    ((CARD32) ((((l) & 0xff000000) >> 24) | \
     (((l) & 0x00ff0000) >> 8)  | \
	 (((l) & 0x0000ff00) << 8)  | \
	 (((l) & 0x000000ff) << 24)))

typedef struct
{
    uint32_t width;
    uint32_t height;
}A;

typedef struct
{
    uint32_t mask;
    uint32_t x;
    uint32_t y;
}B;

void HostToNetwork(uint32_t* address, int length)
{
    if (length % 4)
    {
        return;
    }

    int cnt = length / 4;
    while (cnt--)
    {
        //uint32_t tmp = (uint32_t)(*address);
        //(*address) = Swap32IfLE(tmp);
        (*address) = htonl((*address));
        address++;
    }
}

void NetworkToHost(uint32_t* address, int length)
{
    if (length % 4)
    {
        return;
    }

    int cnt = length / 4;
    uint32_t tmp = 0;
    while (cnt--)
    {
        //tmp = (uint32_t)(*address);
        //(*address) = Swap32IfLE(tmp);
        (*address) = ntohl((*address));
        address++;
    }
}


int main()
{
    std::cout << "Hello World!\n";
    A a = { 1920, 1080 };
    B b = { 1, 100, 200 };
    
    HostToNetwork((uint32_t*)(&a), sizeof(A));
    HostToNetwork((uint32_t*)(&b), sizeof(B));

    HostToNetwork((uint32_t*)(&a), sizeof(A));
    NetworkToHost((uint32_t*)(&b), sizeof(B));

    return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
