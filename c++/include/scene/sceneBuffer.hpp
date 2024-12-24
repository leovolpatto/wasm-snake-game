#pragma once
#include <vector>
#include <memory>
#include "pixel/pixel.hpp"
#include "js/helpers.hpp"
#include "core/types.hpp"
#include "scene/sceneBuffer.hpp"
#include "scene/sceneObject.hpp"
#include "wasm/canvasBuffer.hpp"

class CanvasBuffer;
class SceneObject;

class SceneBuffer
{
private:
    std::unique_ptr<CanvasBuffer> mainBuffer;
    std::vector<std::unique_ptr<SceneObject>> objects;
    int width, height;

public:
    SceneBuffer(int width, int height);
    void addObject(std::unique_ptr<SceneObject> obj);
    const PixelValue* getBuffer() const;
    CanvasBuffer* getMainBuffer();
    void render(float deltaTime);
private:
    void copyObjectToMainBuffer(const SceneObject *obj);
    void renderGrid(int size = 10);
};