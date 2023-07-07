// CompareBMP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <vector>
#include "bmp.h"
#include "desktop_differ_wrapper.h"
#include "../common/common_utils.h"

static int time_duration1 = 0;
static int time_duration2 = 0;

bool DifferTwoFrame(const char* file1, const char* file2);
bool FrameDifferWrapper(const char* file1, const char* file2);
void CompareSameFrames(int rounds, std::vector<bool>& res, bool (*pfun)(const char*, const char*));
void CompareDifferentFrames(int rounds, std::vector<bool>& res, bool (*pfun)(const char*, const char*));

int main()
{
    std::vector<bool> res1;
    std::vector<bool> res2;

    int start = TimeMilliseconds();
    //CompareDifferentFrames(1, res1, DifferTwoFrame);
    CompareSameFrames(5, res1, DifferTwoFrame);
    int duration1 = TimeMilliseconds() - start;
    

    start = TimeMilliseconds();
    //CompareDifferentFrames(1, res2, FrameDifferWrapper);
    CompareSameFrames(5, res2, FrameDifferWrapper);
    int duration2 = TimeMilliseconds() - start;

    std::cout << "HandMade method cost " << duration1 << "ms" << ", pure compare time " << time_duration1 << "ms" << std::endl;
    std::cout << "WebRTC capture differ wrapper method cost " << duration2 << "ms" << ", pure compare time " << time_duration2 << "ms" << std::endl;

    bool is_res_equal = true;
    int same_frame_cnt = 0;
    if (res1.size() == res2.size())
    {
        for (int i = 0; i < res1.size(); i++)
        {
            if (res1[i] != res2[i])
            {
                is_res_equal = false;
                break;
            }
            if (res1[i] == res2[i] && res1[i] == false)
            {
                same_frame_cnt++;
            }
        }
    }
    else
    {
        is_res_equal = false;
        std::cout << "Compare results size is not equal" << std::endl;
    }

    std::cout << "Compared " << res1.size() << " times, " << same_frame_cnt << " times are same frame" << ", all results are equal = " << is_res_equal << std::endl;
    return 0;
}

void CompareSameFrames(int rounds, std::vector<bool>& res, bool (*pfun)(const char*, const char*))
{
    for (int r = 0; r < rounds; r++)
    {
        for (int i = 1; i <= 20; i++)
        {
            char file[60];
            sprintf_s(file, "../../data/nv12_src_%02d.bmp", i);
            bool result = pfun(file, file);
            res.push_back(result);
        }
    }
}

void CompareDifferentFrames(int rounds, std::vector<bool>& res, bool (*pfun)(const char*, const char*))
{
    for (int r = 0; r < rounds; r++)
    {
        for (int i = 1; i <= 20; i++)
        {
            for (int j = i; j <= 20; j++)
            {
                char file1[60];
                char file2[60];
                sprintf_s(file1, "../../data/nv12_src_%02d.bmp", i);
                sprintf_s(file2, "../../data/nv12_src_%02d.bmp", j);
                bool result = pfun(file1, file2);
                res.push_back(result);
            }
        }
    }
}

bool DifferTwoFrame(const char* file1, const char* file2)
{
    int width1, height1;
    int width2, height2;

    uint8_t* image1 = LoadBmpFile(file1, width1, height1);
    uint8_t* image2 = LoadBmpFile(file2, width2, height2);

    uint64_t* src = (uint64_t*)image1;
    uint64_t* dst = (uint64_t*)image2;

    int cnt = 0;

    if (width1 != width2 || height1 != height2)
    {
        cnt = 1;
    }
    else
    {
        int start = TimeMilliseconds();
        for (int i = 0; i < width1; i++)
        {
            for (int j = 0; j < height1 / 2; j++)
            {
                if ((*src++) == (*dst++))
                {
                    continue;
                }
                else
                {
                    //std::cout << "find difference h " << i << " w " << j << std::endl;
                    cnt++;
                    break;
                }
            }
        }
        time_duration1 += TimeMilliseconds() - start;
        //std::cout << "difference count: " << cnt << std::endl;
    }

    if (image1)
    {
        delete[] image1;
    }
    if (image2)
    {
        delete[] image2;
    }
    return (cnt != 0);
}

bool FrameDifferWrapper(const char* file1, const char* file2)
{
    int width1, height1;
    int width2, height2;

    uint8_t* image1 = LoadBmpFile(file1, width1, height1);
    uint8_t* image2 = LoadBmpFile(file2, width2, height2);

    uint64_t* src = (uint64_t*)image1;
    uint64_t* dst = (uint64_t*)image2;

    bool is_differ = false;

    if (width1 != width2 || height1 != height2)
    {
        is_differ = true;
    }
    else
    {
        int stride = width1 * 4;
        int start = TimeMilliseconds();
        is_differ = CompareFrames(image1, image2, width1, height1, stride);
        time_duration2 += TimeMilliseconds() - start;
    }

    if (image1)
    {
        delete[] image1;
    }
    if (image2)
    {
        delete[] image2;
    }

    return is_differ;
}
