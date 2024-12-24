#include "scene/sceneObject.hpp"
#include "wasm/canvasBuffer.hpp"
#include "scene/sceneObject.hpp"
#include "core/point.hpp"
#include <memory>

SceneObject::SceneObject(int initialWidth, int initialHeight, int initialX, int initialY)
    : position(initialX, initialY), width(initialWidth), height(initialHeight)
{
    pixelBuffer = std::make_unique<CanvasBuffer>(width, height);
}

void SceneObject::setPosition(int newX, int newY)
{
    position.x = newX;
    position.y = newY;
}

void SceneObject::setMovement(std::unique_ptr<MovementBehavior> newMovement)
{
    this->movement = std::move(newMovement);
}

void SceneObject::update(float deltaTime)
{
    if (movement)
    {
        position = this->movement->calculateNextPosition(this->getPosition(), deltaTime);
    }
}
