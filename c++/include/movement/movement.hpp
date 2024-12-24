#pragma once
#include "../core/point.hpp"

class MovementBehavior {
public:
    virtual ~MovementBehavior() = default;
    virtual Point calculateNextPosition(const Point& point, float deltaTime) = 0;
};