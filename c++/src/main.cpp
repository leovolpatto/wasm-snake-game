#include <vector>
#include <random>
#include <string>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <emscripten.h>
#include "../include/js/helpers.hpp"

uint8_t *globalBuffer = nullptr;
uint8_t *backupBuffer = nullptr;

int camWidth;
int camHeight;

EM_JS(void, initializeEngine, (), {
    console.log("Engine initialized");
});

extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void init()
    {
        // consoleLog("Init...");
    }

    EMSCRIPTEN_KEEPALIVE
    void allocateMemoryForWebcam(int width, int height)
    {
        camWidth = width;
        camHeight = height;

        const int memoryNeeded = width * height * 4; //4bytes RGBA
        if (!globalBuffer)
        {
            globalBuffer = (uint8_t *)malloc(memoryNeeded);
            backupBuffer = (uint8_t *)malloc(memoryNeeded);

            std::string message = 
                "Init is allocating buffers of size: " 
                + std::to_string(memoryNeeded) 
                + " for size w: " + std::to_string(width)
                + " h: " + std::to_string(height);
            consoleLog(message.c_str());
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void applySnakeHeadEffect(uint8_t *pixels, int width, int height)
    {
        for (int i = 0; i < width * height * 4; i += 4)
        {
            pixels[i] = pixels[i] * (1.0f);
            pixels[i + 1] = pixels[i + 1] * (1.0f);
            pixels[i + 2] = pixels[i + 2] * (1.0f);
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void initializeBuffer()
    {
        // consoleLog("inicializando memoria...");
    }

    EMSCRIPTEN_KEEPALIVE
    uint8_t *getBufferPointer()
    {
        return globalBuffer;
    }

    EMSCRIPTEN_KEEPALIVE
    void processBuffer(int width, int height)
    {
        const int totalPixels = width * height;
        memcpy(backupBuffer, globalBuffer, totalPixels * 4);

        for (int i = 0; i < totalPixels * 4; i += 4)
        {
            globalBuffer[i] = 255;
            // globalBuffer[i+1] é G
            // globalBuffer[i+2] é B
            // globalBuffer[i+3] é A
        }

        const int thumbWidth = width * 0.3;
        const int thumbHeight = height * 0.3;

            std::string message = 
                "Processing thumb buffer: for size w: " + std::to_string(width)
                + " h: " + std::to_string(height);
            consoleLog(message.c_str());           

        // Para cada pixel da miniatura
        for (int y = 0; y < thumbHeight; y++)
        {
            for (int x = 0; x < thumbWidth; x++)
            {
                // Calcula a posição correspondente na imagem original
                const int srcX = (x * width) / thumbWidth;
                const int srcY = (y * height) / thumbHeight;

                // Índices para os pixels fonte e destino
                const int srcIndex = (srcY * width + srcX) * 4;
                const int dstIndex = (y * width + x) * 4;

                // Copia os valores RGBA
                globalBuffer[dstIndex] = globalBuffer[srcIndex];         // R
                globalBuffer[dstIndex + 1] = globalBuffer[srcIndex + 1]; // G
                globalBuffer[dstIndex + 2] = globalBuffer[srcIndex + 2]; // B
                globalBuffer[dstIndex + 3] = globalBuffer[srcIndex + 3]; // A
            }
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void freeBuffer()
    {
        if (globalBuffer)
        {
            free(globalBuffer);
            globalBuffer = nullptr;
        }

        if (backupBuffer)
        {
            free(backupBuffer);
            backupBuffer = nullptr;
        }        
    }
}

int main()
{
    initializeEngine();
    return 0;
}

/*
#include <emscripten.h>
#include <vector>
#include <random>
#include <stdint.h>
#include <stdlib.h>
#include <string>

uint8_t* globalBuffer = nullptr;
constexpr int MAX_WIDTH = 1920;
constexpr int MAX_HEIGHT = 1080;

EM_JS(void, consoleLog, (const char* str), {
    console.log('[FROM C++]' + UTF8ToString(str));
});

EM_JS(void, drawSnakeHead, (const char* str), {
    console.log(UTF8ToString(str));
});

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void init() {
        consoleLog("Init...");
    }

    EMSCRIPTEN_KEEPALIVE
    void allocateMemory(int memory) {
        if (!globalBuffer) {
            globalBuffer = (uint8_t*)malloc(memory);

            std::string message = "Init is allocating buffer of size: " + std::to_string(memory);
            consoleLog(message.c_str());
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void applySnakeHeadEffect(uint8_t* pixels, int width, int height) {
        for(int i = 0; i < width * height * 4; i += 4) {
            pixels[i] = pixels[i] * (1.0f);
            pixels[i + 1] = pixels[i + 1] * (1.0f);
            pixels[i + 2] = pixels[i + 2] * (1.0f);
        }
    }

    EMSCRIPTEN_KEEPALIVE
    void initializeBuffer() {
        consoleLog("inicializando memoria...");
    }

    EMSCRIPTEN_KEEPALIVE
    uint8_t* getBufferPointer() {
        return globalBuffer;
    }

    EMSCRIPTEN_KEEPALIVE
    void processBuffer(int width, int height) {
        const int totalPixels = width * height;
        for(int i = 0; i < totalPixels * 4; i += 4) {
            globalBuffer[i] = 255;
            // globalBuffer[i+1] é G
            // globalBuffer[i+2] é B
            // globalBuffer[i+3] é A
        }

        const int thumbWidth = width * 0.3;
        const int thumbHeight = height * 0.3;

        // Para cada pixel da miniatura
        for(int y = 0; y < thumbHeight; y++) {
            for(int x = 0; x < thumbWidth; x++) {
                // Calcula a posição correspondente na imagem original
                const int srcX = (x * width) / thumbWidth;
                const int srcY = (y * height) / thumbHeight;

                // Índices para os pixels fonte e destino
                const int srcIndex = (srcY * width + srcX) * 4;
                const int dstIndex = (y * width + x) * 4;

                // Copia os valores RGBA
                globalBuffer[dstIndex] = globalBuffer[srcIndex];     // R
                globalBuffer[dstIndex + 1] = globalBuffer[srcIndex + 1]; // G
                globalBuffer[dstIndex + 2] = globalBuffer[srcIndex + 2]; // B
                globalBuffer[dstIndex + 3] = globalBuffer[srcIndex + 3]; // A
            }
        }

    }

    EMSCRIPTEN_KEEPALIVE
    void freeBuffer() {
        if (globalBuffer) {
            free(globalBuffer);
            globalBuffer = nullptr;
        }
    }



}

int main() {
    return 0;
}
*/