#pragma once
#include "movement.hpp"

class CircularMovement : public MovementBehavior {
private:
    Point center;
    float radius;
    float angularSpeed;
    float currentAngle;

public:
    CircularMovement(Point center, float radius, float speed);
    Point calculateNextPosition(const Point& point, float deltaTime) override;
};