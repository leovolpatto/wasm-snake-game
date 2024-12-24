#pragma once
#include <memory>
#include "scene/sceneBuffer.hpp"
#include "wasm/canvasBuffer.hpp"

#include "snake/game_engine.hpp"
#include "snake/renderer.hpp"
#include "movement/circular.hpp"
#include "movement/linear.hpp"

class SceneBuffer;

class Game {
private:
    std::unique_ptr<CanvasBuffer> videoBuffer;
    std::unique_ptr<CanvasBuffer> webcamBuffer;
    std::unique_ptr<SceneBuffer> sceneBuffer;
    
    // Snake game specific members
    std::unique_ptr<snake::GameEngine> snakeEngine;
    std::unique_ptr<snake::Renderer> snakeRenderer;
    snake::GameConfig gameConfig;
    
public:
    Game() = default;
    ~Game() = default;

    void initVideoBuffers(int width, int height);
    void initSceneBuffer(int width, int height);    
    void init(int width, int height, int cellSize);    
    void restart();    
    void play();    
    void pause();    
    void onUserPressedKey(int key);    
    void processSceneBuffer(float deltaTime);    
    void processWebcamBuffer(float deltaTime);    
    void processVideoBuffer(float deltaTime);    
    void setBuffers(std::unique_ptr<SceneBuffer> scene, 
                   std::unique_ptr<CanvasBuffer> video, 
                   std::unique_ptr<CanvasBuffer> webcam);
    
    CanvasBuffer* getVideoBuffer() { return videoBuffer.get(); }
    CanvasBuffer* getWebcamBuffer() { return webcamBuffer.get(); }
    SceneBuffer* getSceneBuffer() { return sceneBuffer.get(); }
    
    void updatePixelsFromBuffer(const uint8_t *backupBuffer, int sourceWidth, int sourceHeight);
};