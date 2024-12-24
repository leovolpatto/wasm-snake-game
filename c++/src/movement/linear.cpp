#include <cmath>
#include <string>
#include "js/helpers.hpp"
#include "movement/linear.hpp"

LinearMovement::LinearMovement(Point direction, float speed) 
    : direction(direction), speed(speed) {

    this->horzMax = direction.x;
    this->horzMin = 0;

    std::string message = 
        " direction X: "  + std::to_string(direction.x)
        + " direction Y: "  + std::to_string(direction.y);
    consoleLog(message.c_str()); 

    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        this->direction.x /= length;
        this->direction.y /= length;

    std::string message3 = 
        "AFTER NORMALIZATION - direction X: " + std::to_string(this->direction.x) +
        " direction Y: " + std::to_string(this->direction.y);
    consoleLog(message3.c_str());        
    }

    std::string message2 = "WTF: " + std::to_string(length)
        + " direction X: "  + std::to_string(direction.x)
        + " direction Y: "  + std::to_string(direction.y);

    consoleLog(message2.c_str());    
}

Point LinearMovement::calculateNextPosition(const Point& point, float deltaTime) {
    if(point.x > this->horzMax){
        this->reverse = true;
    }else if(point.x < this->horzMin){
        this->reverse = false;
    }

    if(this->reverse){
        return Point(
            point.x - direction.x * speed * deltaTime,
            point.y - direction.y * speed * deltaTime
        );        
    }

    return Point(
        point.x + direction.x * speed * deltaTime,
        point.y + direction.y * speed * deltaTime
    );
}