#pragma once
#include "types.hpp"
#include <deque>
#include <optional>
#include "wasm/canvasBuffer.hpp"
#include "js/helpers.hpp"

namespace snake
{
    struct SnakeBodySegment
    {
    private:
        std::unique_ptr<CanvasBuffer> segmentFill;

    public:
        GridPosition position;

        const CanvasBuffer *getSegmentFill() const
        {
            return segmentFill.get();
        }

        void setSegmentFill(std::unique_ptr<CanvasBuffer> &&webcamBuffer)
        {
            const CanvasBuffer &sourceBuffer = *webcamBuffer;
            segmentFill = std::make_unique<CanvasBuffer>(sourceBuffer.getWidth(), sourceBuffer.getHeight());
            PixelValue *dstBuffer = segmentFill->getBufferWritable();

            int bufferSize = sourceBuffer.getBufferSize();
            std::memcpy(dstBuffer, sourceBuffer.getBuffer(), bufferSize * sizeof(PixelValue));
        }

        SnakeBodySegment() = default;
    };

    class Snake
    {
    private:
        const GridDimensions &dimensions_;
        std::deque<SnakeBodySegment> bodySegments_;
        InputKeys currentDirection_;
        std::optional<InputKeys> nextDirection_;
        bool growing_;

        static bool isOppositeDirection(InputKeys a, InputKeys b) noexcept
        {
            return static_cast<int>(a) == (static_cast<int>(b) + 2) % 4;
        }

        GridPosition calculateNewHead() const noexcept
        {
            GridPosition newHead = bodySegments_.front().position;
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

    public:
        explicit Snake(const GridDimensions &dims, GridPosition startPos = {0, 0})
            : dimensions_(dims), bodySegments_(), currentDirection_(InputKeys::Right), growing_(false)
        {
            SnakeBodySegment segment;
            segment.position = startPos;
            bodySegments_.push_front(std::move(segment));
        }

        void setDirection(InputKeys newDirection) noexcept
        {
            if (isOppositeDirection(newDirection, currentDirection_))
            {
                return;
            }
            nextDirection_ = newDirection;
        }

        void grow(std::unique_ptr<CanvasBuffer> &&webcamBuffer)
        {
            consoleLog("Crescendo cobra...");

            // Criamos um novo segmento
            SnakeBodySegment newSegment;
            newSegment.position = bodySegments_.back().position;
            newSegment.setSegmentFill(std::move(webcamBuffer));

            // Adicionamos o novo segmento
            bodySegments_.push_back(std::move(newSegment));
            growing_ = true;

            // Debug
            consoleLog(("Tamanho após grow: " + std::to_string(bodySegments_.size())).c_str());
        }

        void move()
        {
            if (nextDirection_)
            {
                currentDirection_ = *nextDirection_;
                nextDirection_.reset();
            }

            // Criamos uma nova head sem buffer
            SnakeBodySegment newHead;
            newHead.position = calculateNewHead();

            // Adicionamos a nova head no início
            bodySegments_.push_front(std::move(newHead));

            // Se não estiver crescendo, remove o último
            if (!growing_)
            {
                bodySegments_.pop_back();
            }
            else
            {
                growing_ = false;
            }

            // Debug
            size_t count = 0;
            for (const auto &segment : bodySegments_)
            {
                if (segment.getSegmentFill() != nullptr)
                {
                    count++;
                }
            }
            consoleLog(("Buffers após move: " + std::to_string(count) +
                        " de " + std::to_string(bodySegments_.size()))
                           .c_str());
        }

        [[nodiscard]]
        bool checkCollision() const noexcept
        {
            const auto &head = bodySegments_.front().position;

            // Check wall collision
            if (!dimensions_.isValidPosition(head))
            {
                return true;
            }

            // Check self collision
            return std::any_of(bodySegments_.begin() + 1, bodySegments_.end(),
                               [&head](const auto &segment)
                               {
                                   return segment.position == head;
                               });
        }

        [[nodiscard]]
        const std::deque<SnakeBodySegment> &getBodySegments() const noexcept
        {
            return bodySegments_;
        }
    };
}