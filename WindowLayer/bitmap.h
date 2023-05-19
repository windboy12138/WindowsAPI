#pragma once
#include "framework.h"

BYTE* color_bits = nullptr;

void InitColorBitMap(int width, int height, bool top2bottom, int bit_count, BITMAPINFO& bmi)
{
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = top2bottom ? -height : height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = bit_count;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = (width * height * bit_count) / 8;
    memset(bmi.bmiColors, 0, sizeof(RGBQUAD));
}

HICON GetCursorIcon()
{
    CURSORINFO ci = { 0 };
    ci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&ci);
    if (!ci.hCursor)
    {
        return nullptr;
    }

    HICON hIcon = CopyIcon(ci.hCursor);
    if (!hIcon)
    {
        return nullptr;
    }

    return hIcon;
}


HBITMAP GetCursorBMP()
{
    CURSORINFO ci = { 0 };
    ci.cbSize = sizeof(CURSORINFO);
    GetCursorInfo(&ci);
    if (!ci.hCursor)
    {
        return nullptr;
    }

    HICON hIcon = CopyIcon(ci.hCursor);
    if (!hIcon)
    {
        return nullptr;
    }

    ICONINFO info = { 0 };
    if (!GetIconInfo(hIcon, &info))
    {
        return nullptr;
    }

    BITMAP bm = { 0 };
    GetObject(info.hbmMask, sizeof(bm), &bm);
    int nWidth = bm.bmWidth;
    int nHeight = bm.bmHeight;

    HDC color_dc = CreateCompatibleDC(NULL);
    BITMAPINFO bmi = { 0 };
    InitColorBitMap(nWidth, nHeight, true, 32, bmi);

    do
    {
        if (color_bits)
        {
            delete[] color_bits;
        }
        color_bits = new BYTE[bmi.bmiHeader.biSizeImage];
        if (color_bits == nullptr)
        {
            // request memory failed
            break;
        }
        memset(color_bits, 0, bmi.bmiHeader.biSizeImage);

        if (!GetDIBits(color_dc, info.hbmColor, 0, 32, color_bits, &bmi, DIB_RGB_COLORS))
        {
            // failed
            break;
        }
    } while (0);

    DeleteObject(info.hbmMask);
    DeleteObject(info.hbmColor);

    HBITMAP color_bmp = CreateBitmap(nWidth, 32, 1, 32, color_bits);

    return color_bmp;
}