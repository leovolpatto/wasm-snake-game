#pragma once
#include <deque>
#include <optional>
#include "types.hpp"
#include "assets/snake_head.hpp"

namespace snake
{
    struct SnakeSegment
    {
        GridPosition position;
        std::unique_ptr<CanvasBuffer> pixels;

        SnakeSegment(const GridPosition &pos) : position(pos) {}
        SnakeSegment(const GridPosition &pos, std::unique_ptr<CanvasBuffer> pix)
            : position(pos), pixels(std::move(pix)) {}

        bool operator==(const SnakeSegment &other) const
        {
            return position == other.position;
        }
    };

    class Snake
    {
    public:
        explicit Snake(const GridDimensions &dims, GridPosition startPos = {0, 0})
            : dimensions_(dims),
              currentDirection_(InputKeys::Right)
        {
            const size_t maxSize = dims.columns * dims.rows;
            body_.reserve(maxSize);

            auto headTexture = loadSnakeHead(100);
            if (!headTexture)
            {
                throw std::runtime_error("Failed to load snake head texture");
            }

            body_.push_back(SnakeSegment(startPos, std::move(headTexture)));
        }

        void grow(std::unique_ptr<CanvasBuffer> pixels = nullptr) noexcept
        {
            GridPosition pos = body_.at(0).position;
            SnakeSegment newSegment = SnakeSegment(pos);
            newSegment.pixels = std::move(pixels);

            body_.insert(body_.begin() + 1, std::move(newSegment));
        }

        void move()
        {
            if (nextDirection_)
            {
                currentDirection_ = *nextDirection_;
                nextDirection_.reset();
            }

            GridPosition newHead = calculateNewHead();

            for (size_t i = this->getLength() - 1; i > 0; --i)
            {
                body_[i].position = body_[i - 1].position;
            }

            body_[0].position = newHead;
        }

        void setDirection(InputKeys newDirection) noexcept
        {
            if (!isOppositeDirection(newDirection, currentDirection_))
            {
                nextDirection_ = newDirection;
            }

            updateHeadRotation(newDirection);
        }

        [[nodiscard]]
        bool checkCollision() const noexcept
        {
            const auto &head = body_[0];

            if (!dimensions_.isValidPosition(head.position))
            {
                return true;
            }

            for (size_t i = 1; i < this->getLength(); ++i)
            {
                if (body_[i].position == head.position)
                {
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]]
        const SnakeSegment &getHead() const noexcept
        {
            return body_[0];
        }

        [[nodiscard]]
        const std::vector<SnakeSegment> &getBody() const noexcept
        {
            return body_;
        }

        [[nodiscard]]
        size_t getLength() const noexcept
        {

            return body_.size();
        }

    private:
        bool isMovingToRight = false;
        const GridDimensions &dimensions_;
        std::vector<SnakeSegment> body_;
        InputKeys currentDirection_;
        std::optional<InputKeys> nextDirection_;        

        static bool isOppositeDirection(InputKeys a, InputKeys b) noexcept
        {
            return static_cast<int>(a) == (static_cast<int>(b) + 2) % 4;
        }

        std::unique_ptr<CanvasBuffer> loadSnakeHead(int cellSize) const
        {
            auto texture = std::make_unique<CanvasBuffer>(cellSize, cellSize);
            float scaleX = static_cast<float>(cellSize) / assets::SNAKE_HEAD_WIDTH;
            float scaleY = static_cast<float>(cellSize) / assets::SNAKE_HEAD_HEIGHT;

            for (int y = 0; y < cellSize; y++)
            {
                for (int x = 0; x < cellSize; x++)
                {
                    int srcX = static_cast<int>(x / scaleX);
                    int srcY = static_cast<int>(y / scaleY);

                    size_t index = (srcY * assets::SNAKE_HEAD_WIDTH + srcX) * 4;
                    texture->setPixel(
                        x, y,
                        assets::SNAKE_HEAD_DATA[index],
                        assets::SNAKE_HEAD_DATA[index + 1],
                        assets::SNAKE_HEAD_DATA[index + 2],
                        assets::SNAKE_HEAD_DATA[index + 3]);
                }
            }

            return texture;
        }

        GridPosition calculateNewHead()
        {
            GridPosition newHead = body_[0].position;
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
                updateHeadRotation(InputKeys::Left);
                break;
            case InputKeys::Right:
                ++newHead.x;
                updateHeadRotation(InputKeys::Right);
                break;
            default:
                break;
            }

            return newHead;
        }

        void updateHeadRotation(InputKeys newDirection)
        {
            if (newDirection == InputKeys::Left && this->isMovingToRight == false)
            {
                return;
            }

            if (newDirection == InputKeys::Right && this->isMovingToRight == true)
            {
                return;
            }

            if (body_.empty() || !body_[0].pixels)
            {
                return;
            }

            auto &headBuffer = body_[0].pixels;
            if (newDirection == InputKeys::Left)
            {
                this->isMovingToRight = false;
                headBuffer->flipHorizontal();
                return;
            }

            if (newDirection == InputKeys::Right)
            {
                this->isMovingToRight = true;
                headBuffer->flipHorizontal();
            }
        }
    };
}