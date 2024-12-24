#include <iostream>

class Square {
private:
    int posX, posY, width, heigth;

public:
    Square(int x, int y, int w, int h)
        : posX(x), posY(y), width(w), heigth(h) {
        }
    }

    // Métodos de acesso (Getters)
    int getX() const { return x_; }
    int getY() const { return y_; }
    int getSize() const { return size_; }

    // Métodos de modificação (Setters)
    void setPosition(int x, int y) {
        x_ = x;
        y_ = y;
    }

    void setSize(int size) {
        if (size <= 0) {
            throw std::invalid_argument("Size must be greater than zero.");
        }
        size_ = size;
    }

    // Métodos utilitários
    void move(int dx, int dy) {
        x_ += dx;
        y_ += dy;
    }

    void print() const {
        std::cout << "SquareCell(Position: (" << x_ << ", " << y_
                  << "), Size: " << size_ << ")" << std::endl;
    }
};
