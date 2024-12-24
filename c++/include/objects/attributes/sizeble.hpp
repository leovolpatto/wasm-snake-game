#pragma once

class Sizeble {
private:
    int width, height;

public:
    Sizeble(int w, int h): width(w), height(h){};
    virtual ~Sizeble() = default;
    
};