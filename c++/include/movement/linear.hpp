#pragma once
#include "movement.hpp"

class LinearMovement : public MovementBehavior {
private:
    Point direction;  // Direção normalizada do movimento
    float speed;      // Velocidade em unidades por segundo
    int horzMax, horzMin;
    bool reverse = false;

public:
    LinearMovement(Point direction, float speed);
    Point calculateNextPosition(const Point& point, float deltaTime) override;
    const Point& getDirection() const { return direction; }
    float getSpeed() const { return speed; }
    void setSpeed(float newSpeed) { speed = newSpeed; }
    void setDirection(Point newDirection);
};