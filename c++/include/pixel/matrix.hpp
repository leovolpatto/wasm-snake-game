// include/pixel/matrix.hpp
#pragma once
#include "../core/types.hpp"  // para usar sua definição existente de Pixel
#include <vector>
#include <memory>

class PixelMatrix {
private:
    std::vector<std::vector<PixelValue>> matrix;
    int width;
    int height;

public:
    PixelMatrix(int w, int h) : width(w), height(h) {
        matrix.resize(height, std::vector<PixelValue>(width * 4));  // * 4 para RGBA
    }

    // Acesso aos componentes RGBA
    void setPixel(int y, int x, PixelValue r, PixelValue g, PixelValue b, PixelValue a) {
        int baseIdx = x * 4;
        matrix[y][baseIdx] = r;
        matrix[y][baseIdx + 1] = g;
        matrix[y][baseIdx + 2] = b;
        matrix[y][baseIdx + 3] = a;
    }

    // Obtém os componentes RGBA
    void getPixel(int y, int x, PixelValue& r, PixelValue& g, PixelValue& b, PixelValue& a) const {
        int baseIdx = x * 4;
        r = matrix[y][baseIdx];
        g = matrix[y][baseIdx + 1];
        b = matrix[y][baseIdx + 2];
        a = matrix[y][baseIdx + 3];
    }

    // Preenche uma região com uma cor
    void fillRect(int x, int y, int w, int h, PixelValue r, PixelValue g, PixelValue b, PixelValue a) {
        for (int i = y; i < y + h && i < height; i++) {
            for (int j = x; j < x + w && j < width; j++) {
                setPixel(i, j, r, g, b, a);
            }
        }
    }

    // Converte para buffer linear
    std::unique_ptr<PixelValue[]> toBuffer() const {
        auto buffer = std::make_unique<PixelValue[]>(width * height * 4);
        int idx = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int baseIdx = x * 4;
                for (int i = 0; i < 4; i++) {
                    buffer[idx++] = matrix[y][baseIdx + i];
                }
            }
        }
        return buffer;
    }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
};