#pragma once
#include "../js/helpers.hpp"
#include "../objects/game.hpp"
#include <vector>
#include <memory>
#include "../wasm/canvasBuffer.hpp"
#include "../movement/movement.hpp"

class CanvasBuffer;

class SceneObject {
private:
    std::unique_ptr<CanvasBuffer> pixelBuffer;
    Point position;
    int width, height;
    std::unique_ptr<MovementBehavior> movement;

public:
    SceneObject(int initialWidth, int initialHeight, int initialX, int initialY) ;

    void setPosition(int newX, int newY);
    void update(float deltaTime);
    void setMovement(std::unique_ptr<MovementBehavior> newMovement);
    const Point& getPosition() const { return position; };
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    CanvasBuffer* getBuffer() const { return pixelBuffer.get(); }
};