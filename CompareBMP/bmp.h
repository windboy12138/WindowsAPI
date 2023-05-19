#pragma once
#include <Windows.h>
#include <fstream>
#include <windef.h>

uint8_t* LoadBmpFile(const char *file_name)
{
    std::ifstream infile(file_name, std::ios::binary);
    if (!infile)
    {
        std::cout << "File not found." << std::endl;
        return nullptr;
    }

    BITMAPFILEHEADER bmpFileHeader;
    infile.read(reinterpret_cast<char*>(&bmpFileHeader), sizeof(bmpFileHeader));
    std::cout << "File size: " << bmpFileHeader.bfSize << std::endl;


    if (bmpFileHeader.bfType != 0x4D42)//校验：bfType是否为“BM” 0x424D
    {
        return nullptr;
    }

    BITMAPINFOHEADER bmpInfoHeader;
    infile.read(reinterpret_cast<char*>(&bmpInfoHeader), sizeof(bmpInfoHeader));
    int width = bmpInfoHeader.biWidth;
    int height = bmpInfoHeader.biHeight;
    int byte_count = bmpInfoHeader.biBitCount / 8;
    int lineByte = (width * byte_count + 3) / 4 * 4;
    std::cout << "Image size: " << width << " x " << height << std::endl;

    uint8_t* src = new uint8_t[width * height * 3]; // 24 bits true color
    uint8_t* data = src;

    for (int i = 0; i < height; i++)
    {
        infile.read((char*)(data + i * width * byte_count), width * byte_count);//按行读入数据
        infile.seekg(lineByte - width * byte_count, std::ios::cur);//跳过尾部填充区
    }

    uint8_t* dst = new uint8_t[width * height * 4];
    uint8_t* res = dst;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            *dst++ = *data++;
            *dst++ = *data++;
            *dst++ = *data++;
            *dst++ = 0;
        }
    }
    delete[] src;

    infile.close();
    return res;
}