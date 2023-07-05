#pragma once

#include "differ_block.h"
#include "differ_vector_sse2.h"
#include "cpu_features_wrapper.h"

#pragma comment(lib, "system_wrappers.lib")
#pragma comment(lib, "desktop_capture_generic.lib")
#pragma comment(lib, "desktop_capture_differ_sse2.lib")

bool CompareFrames(const uint8_t* image1,
                   const uint8_t* image2,
                   const int width,
                   const int height,
                   const int stride);