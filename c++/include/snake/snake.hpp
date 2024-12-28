#pragma once
#include "types.hpp"
#include <deque>
#include <optional>

namespace snake
{
    class Snake
    {
    public:
        explicit Snake(const GridDimensions &dims, GridPosition startPos = {0, 0})
            : dimensions_(dims), body_({startPos}), currentDirection_(InputKeys::Right), growing_(false) {}

        void setDirection(InputKeys newDirection) noexcept
        {
            // Prevent 180-degree turns
            if (isOppositeDirection(newDirection, currentDirection_))
            {
                return;
            }
            nextDirection_ = newDirection;
        }

        void grow() noexcept
        {
            growing_ = true;
        }

        void move()
        {
            if (nextDirection_)
            {
                currentDirection_ = *nextDirection_;
                nextDirection_.reset();
            }

            GridPosition newHead = calculateNewHead();
            body_.push_front(newHead);

            if (!growing_)
            {
                body_.pop_back();
            }
            else
            {
                growing_ = false;
            }
        }

        [[nodiscard]]
        bool checkCollision() const noexcept
        {
            const auto &head = body_.front();

            // Check wall collision
            if (!dimensions_.isValidPosition(head))
            {
                return true;
            }

            // Check self collision
            return std::any_of(body_.begin() + 1, body_.end(),
                               [&head](const auto &segment)
                               { return segment == head; });
        }

        [[nodiscard]]
        const std::deque<GridPosition> &getBody() const noexcept
        {
            return body_;
        }

    private:
        static bool isOppositeDirection(InputKeys a, InputKeys b) noexcept
        {
            return static_cast<int>(a) == (static_cast<int>(b) + 2) % 4;
        }

        GridPosition calculateNewHead() const noexcept
        {
            GridPosition newHead = body_.front();
            switch (currentDirection_)
            {
            case InputKeys::Up:
                --newHead.y;
                break;
            case InputKeys::Down:
                ++newHead.y;
                break;
            case InputKeys::Left:
                --newHead.x;
                break;
            case InputKeys::Right:
                ++newHead.x;
                break;
            default:
                break;
            }
            return newHead;
        }

        const GridDimensions &dimensions_;
        std::deque<GridPosition> body_;
        InputKeys currentDirection_;
        std::optional<InputKeys> nextDirection_;
        bool growing_;
    };

}