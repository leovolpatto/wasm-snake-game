#pragma once

struct Point {
    float x, y;
    
    Point(float x = 0, float y = 0) : x(x), y(y) {}
    
    Point operator+(const Point& other) const noexcept {
        return Point(x + other.x, y + other.y);
    }
    
    Point operator-(const Point& other) const noexcept {
        return Point(x - other.x, y - other.y);
    }
};