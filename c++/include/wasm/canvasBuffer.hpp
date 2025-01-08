#pragma once
#include <emscripten.h>
#include "../js/helpers.hpp"
#include "../core/types.hpp"
#include <cstring>
#include <iostream>
#include <memory>

class CanvasBuffer
{
private:
    std::unique_ptr<PixelValue[]> buffer;
    int width;
    int height;
    int bufferSize;

public:
    bool initialized;

    CanvasBuffer(int w, int h);

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getBufferSize() const { return width * height * 4; } // 4 para RBGA
    void printInfo() const;
    void clear();
    void copyFrom(const CanvasBuffer *source);
    const PixelValue *getBuffer() const;
    PixelValue *getBufferWritable();
    void setPixel(int x, int y, PixelValue r, PixelValue g, PixelValue b, PixelValue a);
    void getPixel(int x, int y, PixelValue &r, PixelValue &g, PixelValue &b, PixelValue &a) const;
    void fillRect(int x, int y, int w, int h, PixelValue r, PixelValue g, PixelValue b, PixelValue a);
    void copyFromArray(const PixelValue *data, int size);
    BufferInfo *getInfo();

    void copyRect(const CanvasBuffer &source, int destX, int destY, int width, int height)
    {
        float scaleX = (float)source.width / width;
        float scaleY = (float)source.height / height;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int srcX = (int)(x * scaleX);
                int srcY = (int)(y * scaleY);

                int srcIndex = (srcY * source.width + srcX) * 4;
                int destIndex = ((destY + y) * this->width + (destX + x)) * 4;

                this->buffer[destIndex] = source.buffer[srcIndex];
                this->buffer[destIndex + 1] = source.buffer[srcIndex + 1];
                this->buffer[destIndex + 2] = source.buffer[srcIndex + 2];
                this->buffer[destIndex + 3] = source.buffer[srcIndex + 3];
            }
        }
    }

    void blendRect(const CanvasBuffer &source, int destX, int destY, int width, int height, float alpha)
    {
        float scaleX = (float)source.width / width;
        float scaleY = (float)source.height / height;

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                int srcX = (int)(x * scaleX);
                int srcY = (int)(y * scaleY);

                int srcIndex = (srcY * source.width + srcX) * 4;
                int destIndex = ((destY + y) * this->width + (destX + x)) * 4;

                for (int i = 0; i < 4; i++)
                {
                    this->buffer[destIndex + i] =
                        (uint8_t)(this->buffer[destIndex + i] * (1 - alpha) +
                                  source.buffer[srcIndex + i] * alpha);
                }
            }
        }
    }

    void rotateClockwise()
    {
        auto tempBuffer = std::make_unique<uint8_t[]>(width * height * 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                uint8_t r, g, b, a;
                getPixel(x, y, r, g, b, a);

                int newX = height - 1 - y;
                int newY = x;
                int newIndex = (newY * width + newX) * 4;

                tempBuffer[newIndex] = r;
                tempBuffer[newIndex + 1] = g;
                tempBuffer[newIndex + 2] = b;
                tempBuffer[newIndex + 3] = a;
            }
        }

        buffer = std::move(tempBuffer);
        std::swap(width, height);
    }

    void rotateCounterClockwise()
    {
        auto tempBuffer = std::make_unique<uint8_t[]>(width * height * 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                uint8_t r, g, b, a;
                getPixel(x, y, r, g, b, a);

                int newX = y;
                int newY = width - 1 - x;
                int newIndex = (newY * width + newX) * 4;

                tempBuffer[newIndex] = r;
                tempBuffer[newIndex + 1] = g;
                tempBuffer[newIndex + 2] = b;
                tempBuffer[newIndex + 3] = a;
            }
        }

        buffer = std::move(tempBuffer);
        std::swap(width, height);
    }

    void flipHorizontal()
    {
        auto tempBuffer = std::make_unique<uint8_t[]>(width * height * 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                uint8_t r, g, b, a;
                getPixel(x, y, r, g, b, a);

                int newX = width - 1 - x;
                int newY = y;
                int newIndex = (newY * width + newX) * 4;

                tempBuffer[newIndex] = r;
                tempBuffer[newIndex + 1] = g;
                tempBuffer[newIndex + 2] = b;
                tempBuffer[newIndex + 3] = a;
            }
        }

        buffer = std::move(tempBuffer);
    }

    void flipVertical()
    {
        auto tempBuffer = std::make_unique<uint8_t[]>(width * height * 4);

        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                uint8_t r, g, b, a;
                getPixel(x, y, r, g, b, a);

                int newX = x;
                int newY = height - 1 - y;
                int newIndex = (newY * width + newX) * 4;

                tempBuffer[newIndex] = r;
                tempBuffer[newIndex + 1] = g;
                tempBuffer[newIndex + 2] = b;
                tempBuffer[newIndex + 3] = a;
            }
        }

        buffer = std::move(tempBuffer);
    }
};