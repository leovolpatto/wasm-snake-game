#include <cmath>
#include "movement/circular.hpp"

CircularMovement::CircularMovement(Point center, float radius, float speed)
    : center(center), radius(radius), angularSpeed(speed), currentAngle(0) {}

Point CircularMovement::calculateNextPosition(const Point& point, float deltaTime) {
    currentAngle += angularSpeed * deltaTime;
    return Point(
        center.x + radius * cos(currentAngle),
        center.y + radius * sin(currentAngle)
    );
}