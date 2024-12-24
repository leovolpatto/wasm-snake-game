#pragma once
#include <emscripten.h>
#include "core/types.hpp"

extern "C" {
    EMSCRIPTEN_KEEPALIVE void initVideoBuffers(int width, int height);
    EMSCRIPTEN_KEEPALIVE void initSceneBuffer(int width, int height);
    EMSCRIPTEN_KEEPALIVE BufferInfo *getSceneBufferInfo();
    EMSCRIPTEN_KEEPALIVE BufferInfo *getWebcamBufferInfo();
    EMSCRIPTEN_KEEPALIVE BufferInfo *getVideoBufferInfo();
    EMSCRIPTEN_KEEPALIVE void processSceneBuffer(float deltaTime);
    EMSCRIPTEN_KEEPALIVE void processWebcamBuffer(float deltaTime);
    EMSCRIPTEN_KEEPALIVE void processVideoBuffer(float deltaTime);
    EMSCRIPTEN_KEEPALIVE void cleanupBuffers();
}