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
                    config.bufferDimension.height / config.dimensions.rows
                );
                
                buffer_ = std::make_unique<CanvasBuffer>(
                    config.dimensions.columns * cellSize_,
                    config.dimensions.rows * cellSize_
                );

                std::string message2 = "(Renderer) Cell size: " + std::to_string(cellSize_);
                consoleLog(message2.c_str());    

                std::string message = "(Renderer) buffer config dimensions: w: "
                                    + std::to_string(config.bufferDimension.width) + " h: " + std::to_string(config.bufferDimension.height)
                                    + " cols: " + std::to_string(config.dimensions.columns) + " rows: " + std::to_string(config.dimensions.rows);
                consoleLog(message.c_str());
            }

            void setWebcamBuffer(const CanvasBuffer &buffer){
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
            void drawGrid()
            {
                const auto &color = config_.gridColor;

                // Desenha linhas verticais
                for (GridSize x = 0; x <= config_.dimensions.columns; ++x)
                {
                    int pixelX = x * cellSize_;
                    buffer_->fillRect(
                        pixelX, 
                        0, 
                        1, 
                        buffer_->getHeight(),
                        color.r, color.g, color.b, color.a
                    );
                }

                // Desenha linhas horizontais
                for (GridSize y = 0; y <= config_.dimensions.rows; ++y)
                {
                    int pixelY = y * cellSize_;
                    buffer_->fillRect(
                        0,
                        pixelY,
                        buffer_->getWidth(),
                        1,
                        color.r, color.g, color.b, color.a
                    );
                }
            }

            void drawFood(const Food &food)
            {
                const auto &pos = food.getPosition();
                const auto &color = config_.foodColor;

                const int x = pos.x * cellSize_ + 1; // +1 para não sobrepor a grid
                const int y = pos.y * cellSize_ + 1;
                const int width = cellSize_ - 1; // -1 para não sobrepor a grid
                const int height = cellSize_ - 1;                
                
                buffer_->fillRect(
                    x,
                    y,
                    width,
                    height,
                    color.r, color.g, color.b, color.a
                );
                
                const CanvasBuffer& wcBuffer = webcamBuffer_.value();
                buffer_->copyRect(
                    wcBuffer,
                    x + 4, y + 4,
                    width - 8,
                    height - 8
                );                
            }

            void drawSnake(const Snake &snake)
            {
                const auto &color = config_.snakeColor;
                const CanvasBuffer& wcBuffer = webcamBuffer_.value();

                for (const auto &segment : snake.getBodySegments())
                {
                    const int x = segment.position.x * cellSize_ + 1;
                    const int y = segment.position.y * cellSize_ + 1;
                    const int width = cellSize_ - 2;
                    const int height = cellSize_ - 2;

                    // Primeiro desenha o retângulo da cobra
                    buffer_->fillRect(
                        x, y, width, height,
                        color.r, color.g, color.b, color.a
                    );
              
                    if (const auto* segmentBuffer = segment.getSegmentFill()) {
                        consoleLog("Copiando buffer do segmento");
                        consoleLog(("Dimensões: " + std::to_string(segmentBuffer->getWidth()) + 
                                "x" + std::to_string(segmentBuffer->getHeight())).c_str());
                        
                        buffer_->copyRect(*segmentBuffer, x, y, width, height);
                    }
                    else{
                        buffer_->copyRect(
                            wcBuffer,
                            x, y,
                            width,
                            height
                        );                        
                    }

                    buffer_->copyRect(*segmentBuffer, x, y, width, height);
                    
                    /*
                    buffer_->blendRect(
                        wcBuffer,
                        x, y,        // posição destino
                        x, y,        // posição origem
                        width,       // largura
                        height,      // altura
                        0.5f        // fator de blend (0.0 a 1.0)
                    );
                    */
                }
            }
    };
}