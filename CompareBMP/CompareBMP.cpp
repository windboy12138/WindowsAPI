// CompareBMP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include "bmp.h"

int main()
{
    uint8_t* image1 = LoadBmpFile("../../data/nv12_03.bmp");
    uint8_t* image2 = LoadBmpFile("../../data/nv12_04.bmp");

    uint64_t* src = (uint64_t*)image1;
    uint64_t* dst = (uint64_t*)image2;

    int cnt = 0;

    for (int i = 0; i < 1088; i++)
    {
        for (int j = 0; j < 1920 / 2; j++)
        {
            if ((*src++) == (*dst++))
            {
                continue;
            }
            else
            {
                //std::cout << "find difference h " << i << " w " << j << std::endl;
                cnt++;
            }
        }
    }
    std::cout << "difference count: " << cnt << std::endl;


    if (image1)
    {
        delete[] image1;
    }
    if (image2)
    {
        delete[] image2;
    }
    return 0;
}
