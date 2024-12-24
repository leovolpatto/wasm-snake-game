#pragma once
#include <cstdint>
#include <array>

namespace snake
{
    using GridSize = int32_t;
    using Score = uint32_t;
    using Pixels = uint32_t;

    struct GridPosition
    {
        GridSize x;
        GridSize y;

        constexpr bool operator==(const GridPosition &other) const noexcept
        {
            return x == other.x && y == other.y;
        }

        constexpr bool operator!=(const GridPosition &other) const noexcept
        {
            return !(*this == other);
        }
    };

    struct GridDimensions
    {
        GridSize columns;
        GridSize rows;

        constexpr bool isValidPosition(const GridPosition &pos) const noexcept
        {
            return pos.x >= 0 && pos.x < columns &&
                   pos.y >= 0 && pos.y < rows;
        }
    };

    struct RenderedSceneDimension
    {
        Pixels width;
        Pixels height;
    };    

    enum class InputKeys : uint8_t
    {
        Up,
        Right,
        Down,
        Left,
        Space
    };

    enum class GameState : uint8_t
    {
        Ready,
        Playing,
        Paused,
        GameOver
    };

    struct Color
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
}