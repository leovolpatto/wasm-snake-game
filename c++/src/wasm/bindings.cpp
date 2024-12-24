#include <memory>
#include <cstring>
#include <cmath>
#include "wasm/bindings.hpp"
#include "js/helpers.hpp"
#include "scene/sceneBuffer.hpp"
#include "scene/sceneObject.hpp"
#include "wasm/canvasBuffer.hpp"
#include "movement/circular.hpp"
#include "movement/linear.hpp"

std::unique_ptr<Game> game = nullptr;

extern "C"
{
    EMSCRIPTEN_KEEPALIVE
    void initialize()
    {
        game = std::make_unique<Game>();
        game->init(900, 500, 50);
    }

    EMSCRIPTEN_KEEPALIVE
    void processKeyPressed(int key)
    {
        game->onUserPressedKey(key);
    }

    EMSCRIPTEN_KEEPALIVE
    void initVideoBuffers(int width, int height)
    {
        game->initVideoBuffers(width, height);
    }

    EMSCRIPTEN_KEEPALIVE
    void initSceneBuffer(int width, int height)
    {
        game->initSceneBuffer(width, height);
        game->play();
    }

    EMSCRIPTEN_KEEPALIVE
    void processSceneBuffer(float deltaTime)
    {
        game->processSceneBuffer(deltaTime);
    }

    EMSCRIPTEN_KEEPALIVE
    void processWebcamBuffer(float deltaTime)
    {
        game->processWebcamBuffer(deltaTime);
    }

    EMSCRIPTEN_KEEPALIVE
    void processVideoBuffer(float deltaTime)
    {
        game->processVideoBuffer(deltaTime);
    }    

    EMSCRIPTEN_KEEPALIVE
    BufferInfo *getSceneBufferInfo()
    {
        return game->getSceneBuffer()->getMainBuffer()->getInfo();
    }

    EMSCRIPTEN_KEEPALIVE
    BufferInfo *getWebcamBufferInfo()
    {
        return game->getWebcamBuffer()->getInfo();
    }

    EMSCRIPTEN_KEEPALIVE
    BufferInfo *getVideoBufferInfo()
    {
        return game->getVideoBuffer()->getInfo();
    }

    EMSCRIPTEN_KEEPALIVE
    void cleanupBuffers()
    {
        // sceneBuffer = nullptr;
        // webcamBuffer = nullptr;
        // videoBuffer = nullptr;
    }
}