#pragma once
#include "types.hpp"

namespace snake
{
    struct GameConfig
    {
        GridDimensions dimensions{50, 50}; // Default grid size
        RenderedSceneDimension bufferDimension{1200,900};
        float initialSpeed{5.0f};          // Movements per second
        float speedIncrease{0.5f};         // Speed increase per food eaten
        Color snakeColor{125, 118, 117, 255};   // #7d7675
        Color foodColor{255, 0, 0, 255};
        Color gridColor{50, 50, 50, 255};
    };
}