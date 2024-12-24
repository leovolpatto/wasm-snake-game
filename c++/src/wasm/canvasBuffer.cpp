#include "wasm/canvasBuffer.hpp"

CanvasBuffer::CanvasBuffer(int w, int h)
    : width(w),
      height(h),
      bufferSize(w * h * 4), // RGBA
      initialized(true)
{
    buffer = std::make_unique<PixelValue[]>(bufferSize);
    std::memset(buffer.get(), 0, bufferSize * sizeof(PixelValue));
}

void CanvasBuffer::printInfo() const
{
    std::cout << "Width: " << width << ", Height: " << height << std::endl;
    std::cout << "initialized: " << initialized << std::endl;
}

void CanvasBuffer::clear()
{
    std::memset(buffer.get(), 0, bufferSize * sizeof(PixelValue));
}

void CanvasBuffer::copyFrom(const CanvasBuffer *source)
{
    if (width != source->width || height != source->height)
    {
        consoleLog("Buffer dimensions mismatch");
        return;
    }

    std::memcpy(
        buffer.get(),
        source->getBuffer(),
        bufferSize * sizeof(PixelValue));
}

// Para leitura: retorna ponteiro constante (somente leitura)
const PixelValue *CanvasBuffer::getBuffer() const
{
    return buffer.get();
}

// Para escrita: retorna ponteiro não-constante
PixelValue *CanvasBuffer::getBufferWritable()
{
    return buffer.get();
}

// Para escrita de um pixel específico
void CanvasBuffer::setPixel(int x, int y, PixelValue r, PixelValue g, PixelValue b, PixelValue a)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }

    int index = (y * width + x) * 4;
    buffer[index] = r;
    buffer[index + 1] = g;
    buffer[index + 2] = b;
    buffer[index + 3] = a;
}

// Para leitura de um pixel específico
void CanvasBuffer::getPixel(int x, int y, PixelValue &r, PixelValue &g, PixelValue &b, PixelValue &a) const
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    int index = (y * width + x) * 4;
    r = buffer[index];
    g = buffer[index + 1];
    b = buffer[index + 2];
    a = buffer[index + 3];
}

// Para preencher uma região
void CanvasBuffer::fillRect(int x, int y, int w, int h, PixelValue r, PixelValue g, PixelValue b, PixelValue a)
{
    for (int i = y; i < y + h && i < height; i++)
    {
        for (int j = x; j < x + w && j < width; j++)
        {
            setPixel(j, i, r, g, b, a);
        }
    }
}

// Para copiar dados para o buffer
void CanvasBuffer::copyFromArray(const PixelValue *data, int size)
{
    int maxSize = width * height * 4;
    int copySize = std::min(size, maxSize);
    std::memcpy(buffer.get(), data, copySize * sizeof(PixelValue));
}

BufferInfo *CanvasBuffer::getInfo()
{
    static BufferInfo info;
    info = {
        getBufferWritable(),
        width,
        height,
        bufferSize,
        initialized};
        
    return &info;
}