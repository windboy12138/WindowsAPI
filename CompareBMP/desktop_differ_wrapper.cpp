#include "desktop_differ_wrapper.h"
#include <memory.h>

#pragma comment(lib, "system_wrappers.lib")
#pragma comment(lib, "desktop_capture.lib")
#pragma comment(lib, "desktop_capture_generic.lib")
#pragma comment(lib, "desktop_capture_differ_sse2.lib")

bool PartialBlockDifference(const uint8_t* old_buffer,
                            const uint8_t* new_buffer,
                            int width,
                            int height,
                            int stride) {
    const int width_bytes = width * webrtc::kBytesPerPixel;
    for (int i = 0; i < height; i++) {
        if (memcmp(old_buffer, new_buffer, width_bytes) != 0) {
            return true;
        }
        old_buffer += stride;
        new_buffer += stride;
    }
    return false;
}

bool CompareRow(const uint8_t* old_buffer,
                const uint8_t* new_buffer,
                const int left,
                const int right,
                const int top,
                const int bottom,
                const int stride)
{
    const int block_x_offset = webrtc::kBlockSize * webrtc::kBytesPerPixel;
    const int width = right - left;
    const int height = bottom - top;
    const int block_count = (width - 1) / webrtc::kBlockSize;
    const int last_block_width = width - block_count * webrtc::kBlockSize;

    bool is_differ = false;

    for (int x = 0; x < block_count; x++)
    {
        if (webrtc::BlockDifference(old_buffer, new_buffer, height, stride))
        {
            is_differ = true;
            break;
        }
        else
        {
            old_buffer += block_x_offset;
            new_buffer += block_x_offset;
        }
    }

    if (last_block_width < webrtc::kBlockSize)
    {
        is_differ = PartialBlockDifference(old_buffer, new_buffer, last_block_width, height, stride);
    }
    else
    {
        is_differ = webrtc::BlockDifference(old_buffer, new_buffer, height, stride);
    }

    return is_differ;
}

bool CompareFrames(const uint8_t* image1,
                   const uint8_t* image2,
                   const int width,
                   const int height,
                   const int stride)
{
    const int y_block_count = (height - 1) / webrtc::kBlockSize;
    const int last_y_block_height = height - y_block_count * webrtc::kBlockSize;

    const int block_y_strie = stride * webrtc::kBlockSize;

    const uint8_t* prev_block_row_start = image1;
    const uint8_t* curr_block_row_start = image2;

    int top = 0;
    bool is_differ = false;

    for (int y = 0; y < y_block_count; y++)
    {
        if (CompareRow(prev_block_row_start, curr_block_row_start, 0, width, top, top + webrtc::kBlockSize, stride))
        {
            is_differ = true;
            return is_differ;
        }
        else
        {
            top += webrtc::kBlockSize;
            prev_block_row_start += block_y_strie;
            curr_block_row_start += block_y_strie;
        }
    }

    is_differ = CompareRow(prev_block_row_start, curr_block_row_start, 0, width, top, top + last_y_block_height, stride);

    return is_differ;
}