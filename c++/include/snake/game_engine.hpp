#pragma once
#include <memory>
#include <stdexcept>
#include <functional>
#include "types.hpp"
#include "snake.hpp"
#include "food.hpp"
#include "config.hpp"
#include "js/helpers.hpp"
#include "wasm/canvasBuffer.hpp"

namespace snake
{
    class GameEngine
    {
        private:
            GameConfig config_;
            RandomGenerator rng_;
            std::unique_ptr<Snake> snake_;
            std::unique_ptr<Food> food_;
            GameState state_;
            Score score_;
            float moveTimer_;
            float currentSpeed_;
            std::optional<std::reference_wrapper<const CanvasBuffer>> webcamBuffer_;

            void eatFood(){
                score_++;
                consoleLog("eating...");

                const CanvasBuffer& wcBuffer = webcamBuffer_.value();
                const PixelValue* srcBuffer = wcBuffer.getBuffer();

                std::unique_ptr<CanvasBuffer> thumbBuffer = std::make_unique<CanvasBuffer>(wcBuffer.getWidth(), wcBuffer.getHeight());
                PixelValue* dstBuffer = thumbBuffer->getBufferWritable();
                
                int bufferSize = wcBuffer.getBufferSize();
                std::memcpy(dstBuffer, srcBuffer, bufferSize * sizeof(PixelValue));

                snake_->grow(std::move(thumbBuffer));
                currentSpeed_ += config_.speedIncrease;
            }

            void handleFoodCollision()
            {
                eatFood();

                consoleLog("food collision ok");
                // Try to find a new position for food that's not on the snake
                size_t attempts = 0;
                const size_t maxAttempts = 100; // Prevent infinite loop

                do
                {
                    food_->reposition();
                    attempts++;
                } while (isPositionOnSnake(food_->getPosition()) && attempts < maxAttempts);

                if (attempts >= maxAttempts)
                {
                    // Game is won - snake fills the entire grid
                    state_ = GameState::GameOver;
                }
            }

            bool isPositionOnSnake(const GridPosition &pos) const
            {
                const auto &body = snake_->getBodySegments();
                //return std::find(body.begin(), body.end(), pos) != body.end();

                return std::any_of(body.begin(), body.end(), 
                                [&pos](const auto& segment) { 
                                    return segment.position == pos; 
                                });                
            }

        public:
            //using RenderCallback = std::function<void(const Snake &, const Food &, const GameConfig &)>;

            explicit GameEngine(const GameConfig &config = GameConfig{})
                : config_(config), rng_(), state_(GameState::Ready), score_(0), moveTimer_(0), currentSpeed_(config.initialSpeed)
            {
                reset();
            }

            void setWebcamBuffer(const CanvasBuffer &buffer){
                this->webcamBuffer_ = std::ref(buffer);
            }

            void update(float deltaTime)
            {
                if (state_ != GameState::Playing)
                {
                    return;
                }

                moveTimer_ += deltaTime;
                float moveInterval = 1.0f / currentSpeed_;

                if (moveTimer_ >= moveInterval)
                {
                    moveTimer_ -= moveInterval;
                    snake_->move();

                    if (snake_->checkCollision())
                    {
                        consoleLog("GameOver");
                        state_ = GameState::GameOver;
                        return;
                    }

                    if (snake_->getBodySegments().front().position == food_->getPosition())
                    {
                        handleFoodCollision();
                    }
                }
            }

            void setDirection(InputKeys dir)
            {
                if (state_ == GameState::Playing)
                {
                    snake_->setDirection(dir);
                }
            }

            void togglePause()
            {
                if (state_ == GameState::Playing)
                {
                    state_ = GameState::Paused;
                }
                else if (state_ == GameState::Paused)
                {
                    state_ = GameState::Playing;
                }
            }

            void start()
            {
                if (state_ == GameState::Ready || state_ == GameState::GameOver)
                {
                    reset();
                }
                
                state_ = GameState::Playing;
            }

            void reset()
            {
                GridPosition center{
                    config_.dimensions.columns / 2,
                    config_.dimensions.rows / 2};

                snake_ = std::make_unique<Snake>(config_.dimensions, center);
                food_ = std::make_unique<Food>(rng_, config_.dimensions);
                score_ = 0;
                currentSpeed_ = config_.initialSpeed;
                moveTimer_ = 0;
                state_ = GameState::Ready;
            }

            [[nodiscard]] GameState getState() const noexcept { return state_; }
            [[nodiscard]] Score getScore() const noexcept { return score_; }
            [[nodiscard]] const Snake &getSnake() const noexcept { return *snake_; }
            [[nodiscard]] const Food &getFood() const noexcept { return *food_; }
            [[nodiscard]] const GameConfig &getConfig() const noexcept { return config_; }
    };
}