#pragma once
#include <memory>
#include "game_engine.hpp"
#include "wasm/canvasBuffer.hpp"
#include "js/helpers.hpp"

namespace snake
{
    class Renderer
    {
    public:
        const GameConfig &config_;
        std::optional<std::reference_wrapper<const CanvasBuffer>> webcamBuffer_;
        int cellSize_;
        std::unique_ptr<CanvasBuffer> buffer_;

        explicit Renderer(const GameConfig &config)
            : config_(config)
        {
            cellSize_ = std::min(
                config.bufferDimension.width / config.dimensions.columns,
                config.bufferDimension.height / config.dimensions.rows);

            buffer_ = std::make_unique<CanvasBuffer>(
                config.dimensions.columns * cellSize_,
                config.dimensions.rows * cellSize_);

            std::string message2 = "(Renderer) Cell size: " + std::to_string(cellSize_);
            consoleLog(message2.c_str());

            std::string message = "(Renderer) buffer config dimensions: w: " + std::to_string(config.bufferDimension.width) + " h: " + std::to_string(config.bufferDimension.height) + " cols: " + std::to_string(config.dimensions.columns) + " rows: " + std::to_string(config.dimensions.rows);
            consoleLog(message.c_str());
        }

        void setWebcamBuffer(const CanvasBuffer &buffer)
        {
            this->webcamBuffer_ = std::ref(buffer);
        }

        void render(const GameEngine &engine)
        {
            buffer_->clear();
            drawGrid();
            drawFood(engine.getFood());
            drawSnake(engine.getSnake());
        }

        [[nodiscard]]
        const CanvasBuffer *getBuffer() const noexcept
        {
            return buffer_.get();
        }

    private:
        static constexpr float MIN_ALPHA = 0.2f;
        static constexpr float MAX_ALPHA = 1.0f;

        void drawGrid()
        {
            const auto &color = config_.gridColor;

            for (GridSize x = 0; x <= config_.dimensions.columns; ++x)
            {
                int pixelX = x * cellSize_;
                buffer_->fillRect(
                    pixelX,
                    0,
                    1,
                    buffer_->getHeight(),
                    color.r, color.g, color.b, color.a);
            }

            for (GridSize y = 0; y <= config_.dimensions.rows; ++y)
            {
                int pixelY = y * cellSize_;
                buffer_->fillRect(
                    0,
                    pixelY,
                    buffer_->getWidth(),
                    1,
                    color.r, color.g, color.b, color.a);
            }
        }

        void drawFood(const Food &food)
        {
            const auto &pos = food.getPosition();
            const auto &color = config_.foodColor;
            const CanvasBuffer &wcBuffer = webcamBuffer_.value();

            const int x = pos.x * cellSize_;
            const int y = pos.y * cellSize_;
            const int width = cellSize_;
            const int height = cellSize_;

            buffer_->fillRect(
                x,
                y,
                width, height,
                color.r, color.g, color.b, color.a);

            buffer_->copyRect(
                wcBuffer,
                x + 5,
                y + 5,
                width - 9,
                height - 9);
        }

        

        void drawSnake(const Snake &snake)
        {
            const auto &body = snake.getBody();
            const size_t length = snake.getLength();

            const auto &color = config_.snakeColor;
            const CanvasBuffer &wcBuffer = webcamBuffer_.value();

            for (size_t i = 0; i < length; ++i)
            {
                const auto &segment = body[i];
                int x = segment.position.x * cellSize_ + 1;
                int y = segment.position.y * cellSize_ + 1;
                int width = cellSize_ - 1;
                int height = cellSize_ - 1;

                buffer_->fillRect(
                    x, y, width, height,
                    color.r, color.g, color.b, color.a);


                if (i == 0 && segment.pixels && segment.pixels->getBuffer()) {
                    // Renderiza a cabeça
                    buffer_->copyRect(
                        *segment.pixels,
                        x, y,
                        width,
                        height
                    );
                    continue;
                }

                /*
                if (i == 0)
                {
                    buffer_->fillRect(
                        x, y, width, height,
                        80, 80, 80, 255);

                    buffer_->copyRect(
                        wcBuffer,// *this->snakeHeadTexture_ ,,
                        x - 2, 
                        y -2,
                        width + 4 ,
                        height + 4 );

                    continue;
                }*/

                if (segment.pixels && segment.pixels->getBuffer())
                {
                    float alpha = MAX_ALPHA - ((MAX_ALPHA - MIN_ALPHA) * static_cast<float>(i) / static_cast<float>(length - 1));

                    buffer_->blendRect(
                        *segment.pixels,
                        x, y,
                        width,
                        height,
                        alpha);
                }
            }
        }
    };
}