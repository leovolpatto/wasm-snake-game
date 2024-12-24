#pragma once
#include "types.hpp"
#include "random_generator.hpp"
#include <memory>

namespace snake
{

    class Food
    {
    public:
        explicit Food(RandomGenerator &rng, const GridDimensions &dims)
            : rng_(rng), dimensions_(dims)
        {
            reposition();
        }

        void reposition() noexcept
        {
            position_ = rng_.generatePosition(dimensions_);
        }

        [[nodiscard]]
        const GridPosition &getPosition() const noexcept
        {
            return position_;
        }

    private:
        RandomGenerator &rng_;
        const GridDimensions &dimensions_;
        GridPosition position_;
    };

}