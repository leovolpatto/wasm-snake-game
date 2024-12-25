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
    BufferInfo* getInfo();

    void copyRect(const CanvasBuffer& source, int destX, int destY, int width, int height) {
        float scaleX = (float)source.width / width;
        float scaleY = (float)source.height / height;
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
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

    void blendRect(const CanvasBuffer& source,
                  int destX, int destY,
                  int srcX, int srcY,
                  int width, int height,
                  float alpha) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int srcIndex = ((srcY + y) * source.width + (srcX + x)) * 4;
                int destIndex = ((destY + y) * this->width + (destX + x)) * 4;
                
                for (int i = 0; i < 4; i++) {
                    this->buffer[destIndex + i] = 
                        (uint8_t)(this->buffer[destIndex + i] * (1 - alpha) + 
                                 source.buffer[srcIndex + i] * alpha);
                }
            }
        }
    }    
};