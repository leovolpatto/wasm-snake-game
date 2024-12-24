#pragma once
#include "types.hpp"

namespace snake
{
    struct GameConfig
    {
        GridDimensions dimensions{30, 20}; // Default grid size
        RenderedSceneDimension bufferDimension{900,500};
        float initialSpeed{5.0f};          // Movements per second
        float speedIncrease{0.5f};         // Speed increase per food eaten
        Color snakeColor{0, 255, 0, 255};  // Green
        Color foodColor{255, 0, 0, 255};   // Red
        Color gridColor{50, 50, 50, 255};  // Dark gray
    };
}