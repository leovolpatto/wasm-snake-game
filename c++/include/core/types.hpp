#pragma once
#include <cstdint>

using PixelValue = uint8_t;

struct BufferInfo {
    PixelValue* buffer;
    int width;
    int height;
    int bufferSize;
    bool initialized;
};