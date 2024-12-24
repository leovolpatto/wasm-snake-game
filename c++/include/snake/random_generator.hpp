#pragma once
#include <random>
#include "types.hpp"

namespace snake
{

    class RandomGenerator
    {
        public:
            RandomGenerator() : engine(std::random_device{}()) {}

            GridPosition generatePosition(const GridDimensions &dims)
            {
                std::uniform_int_distribution<GridSize> distX(0, dims.columns - 1);
                std::uniform_int_distribution<GridSize> distY(0, dims.rows - 1);
                return GridPosition{distX(engine), distY(engine)};
            }

        private:
            std::mt19937 engine;
    };

}