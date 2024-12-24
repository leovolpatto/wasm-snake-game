#include <cstdio>
#include <memory>
#include <string>
#include "scene/sceneBuffer.hpp"
#include "scene/sceneObject.hpp"
#include "wasm/canvasBuffer.hpp"

SceneBuffer::SceneBuffer(int width, int height) : width(width), height(height)
{
    mainBuffer = std::make_unique<CanvasBuffer>(width, height);
}

void SceneBuffer::addObject(std::unique_ptr<SceneObject> obj)
{
    std::string message = "Adding object with dimensions: " +
                          std::to_string(obj->getWidth()) + "x" + std::to_string(obj->getHeight());
    consoleLog(message.c_str());

    objects.push_back(std::move(obj));
}

const PixelValue *SceneBuffer::getBuffer() const
{
    return mainBuffer->getBuffer();
}

CanvasBuffer *SceneBuffer::getMainBuffer()
{
    return mainBuffer.get();
}

void SceneBuffer::render(float deltaTime)
{
    mainBuffer->clear();
    /*
    this->renderGrid(20);
    for (const auto &obj : objects)
    {
        obj->update(deltaTime);
        copyObjectToMainBuffer(obj.get());
    }
    */
}

void SceneBuffer::renderGrid(int size)
{
    for (int col = 0; col < this->width; col++)
    {
        for (int lin = 0; lin < this->height; lin += size)
        {
            this->mainBuffer->setPixel(col, lin, 0, 0, 0, 255);
        }
    }

    for (int col = 0; col < this->width; col += size)
    {
        for (int lin = 0; lin < this->height; lin++)
        {
            this->mainBuffer->setPixel(col, lin, 0, 0, 0, 255);
        }
    }
}

void SceneBuffer::copyObjectToMainBuffer(const SceneObject *obj)
{
    // std::string message = "Copying object with dimensions: " +
    //                       std::to_string(objWidth) + "x" + std::to_string(objHeight) + " at x: " + std::to_string(position.x) + " y: " + std::to_string(position.y) + " to the CanvasBuffer with dimensions: " +
    //                       std::to_string(mainWidth) + "x" + std::to_string(mainHeight);
    // consoleLog(message.c_str());

    CanvasBuffer *objBuffer = obj->getBuffer();
    PixelValue *mainPixels = mainBuffer->getBufferWritable();
    const PixelValue *objPixels = objBuffer->getBuffer();

    int objWidth = objBuffer->getWidth();
    int objHeight = objBuffer->getHeight();
    int mainWidth = mainBuffer->getWidth();
    int mainHeight = mainBuffer->getHeight();

    Point position = obj->getPosition();
    int objX = position.x;
    int objY = position.y;

    // Para cada pixel do objeto
    for (int y = 0; y < objHeight; y++)
    {
        for (int x = 0; x < objWidth; x++)
        {
            // Calcular posição no buffer principal
            int destX = objX + x;
            int destY = objY + y;

            // Verificar se está dentro dos limites
            if (destX >= 0 && destX < mainWidth && destY >= 0 && destY < mainHeight)
            {
                int srcIdx = (y * objWidth + x) * 4;
                int dstIdx = (destY * mainWidth + destX) * 4;
                for (int c = 0; c < 4; c++)
                {
                    mainPixels[dstIdx + c] = objPixels[srcIdx + c];
                }
            }
        }
    }
}