#include "objects/game.hpp"
#include "movement/circular.hpp"
#include "movement/linear.hpp"

void Game::initVideoBuffers(int width, int height)
{
    // videoBuffer = std::make_unique<CanvasBuffer>(width, height);
    // webcamBuffer = std::make_unique<CanvasBuffer>(width, height);

    // std::string message = "Video buffers initialized with dimensions: " +
    //                       std::to_string(width) + "x" + std::to_string(height);
    // consoleLog(message.c_str());

    videoBuffer.reset();
    webcamBuffer.reset();

    // Cria novos buffers com as dimensões corretas
    videoBuffer = std::make_unique<CanvasBuffer>(width, height);
    webcamBuffer = std::make_unique<CanvasBuffer>(width, height);

    // Inicializa os buffers com zeros
    videoBuffer->clear();
    webcamBuffer->clear();

    std::string message = "(Game::initVideoBuffers) Video buffers initialized with dimensions: " +
                         std::to_string(width) + "x" + std::to_string(height);
    consoleLog(message.c_str());    
}

void Game::initSceneBuffer(int width, int height)
{
    this->sceneBuffer = std::make_unique<SceneBuffer>(width, height);

    // auto obj = std::make_unique<SceneObject>(50, 50, 0, 0);
    // PixelValue *buffer = obj->getBuffer()->getBufferWritable();
    // int objWidth = obj->getBuffer()->getWidth();
    // int objHeight = obj->getBuffer()->getHeight();

    // auto circularMove = std::make_unique<CircularMovement>(Point(100, 100), 100, 1);
    // obj->setMovement(std::move(circularMove));

    // for (int i = 0; i < objWidth * objHeight * 4; i += 4)
    // {
    //     buffer[i] = 255;     // R
    //     buffer[i + 1] = 0;   // G
    //     buffer[i + 2] = 0;   // B
    //     buffer[i + 3] = 255; // a
    // }

    // sceneBuffer->addObject(std::move(obj));

    // auto obj2 = std::make_unique<SceneObject>(20, 20, 0, 0);
    // PixelValue *buffer2 = obj2->getBuffer()->getBufferWritable();
    // auto linearMovement = std::make_unique<LinearMovement>(Point(800, 0), 80);
    // obj2->setMovement(std::move(linearMovement));
    // int objWidth2 = obj2->getBuffer()->getWidth();
    // int objHeight2 = obj2->getBuffer()->getHeight();
    // for (int i = 0; i < objWidth2 * objHeight2 * 4; i += 4)
    // {
    //     buffer2[i] = 0;       // R
    //     buffer2[i + 1] = 255; // G
    //     buffer2[i + 2] = 0;   // B
    //     buffer2[i + 3] = 255; // a
    // }
    // sceneBuffer->addObject(std::move(obj2));

    std::string message = "(Game::initSceneBuffer) Scene buffer initialized with dimensions: " +
                          std::to_string(width) + "x" + std::to_string(height);
    consoleLog(message.c_str());

    snakeRenderer->setWebcamBuffer(*webcamBuffer);
}

void Game::init(int width, int height, int cellSize)
{    
    gameConfig.dimensions = snake::GridDimensions{
        static_cast<snake::GridSize>(width / cellSize),
        static_cast<snake::GridSize>(height / cellSize)
    };

    std::string message = "(Game::init) Game config dimensions: cols: " +
                          std::to_string(gameConfig.dimensions.columns) + " rows: " + std::to_string(gameConfig.dimensions.rows);
    consoleLog(message.c_str());

    gameConfig.bufferDimension.height = height;
    gameConfig.bufferDimension.width = width;

    std::string message2 = "(Game::init) Game config buffer dimensions: w: " +
                          std::to_string(gameConfig.bufferDimension.width) + " h: " + std::to_string(gameConfig.bufferDimension.height);
    consoleLog(message2.c_str());

    snakeEngine = std::make_unique<snake::GameEngine>(gameConfig);
    snakeRenderer = std::make_unique<snake::Renderer>(gameConfig);
    
    gameConfig.gridColor = {80, 80, 80, 255}; 
    gameConfig.snakeColor = {0, 255, 0, 255}; 
    gameConfig.foodColor = {255, 0, 0, 255}; 
}

void Game::restart()
{
    if (snakeEngine)
    {
        snakeEngine->reset();
    }
}

void Game::play()
{
    if (snakeEngine)
    {
        snakeEngine->start();
    }
}

void Game::pause()
{
    if (snakeEngine)
    {
        snakeEngine->togglePause();
    }
}

void Game::onUserPressedKey(int key)
{
    if (!snakeEngine){
        return;
    }

    snake::InputKeys dir = static_cast<snake::InputKeys>(key);
    switch (dir)
    {
        case snake::InputKeys::Up:
            snakeEngine->setDirection(snake::InputKeys::Up);
            break;
        case snake::InputKeys::Down:
            snakeEngine->setDirection(snake::InputKeys::Down);
            break;
        case snake::InputKeys::Left:            
            snakeEngine->setDirection(snake::InputKeys::Left);
            break;
        case snake::InputKeys::Right:
            snakeEngine->setDirection(snake::InputKeys::Right);
            break;
        default:
            consoleLog("Key pressed: other");
            if (snakeEngine->getState() == snake::GameState::Ready ||
                snakeEngine->getState() == snake::GameState::GameOver)
            {
                snakeEngine->start();
            }
            else
            {
                snakeEngine->togglePause();
            }
            break;
    }
}

void Game::processSceneBuffer(float deltaTime)
{
    if (!snakeEngine || !snakeRenderer || !sceneBuffer) {
        return;
    }

    snakeEngine->update(deltaTime);

    auto* mainBuffer = sceneBuffer->getMainBuffer();
    if (mainBuffer) {
        mainBuffer->clear();
    }

    snakeRenderer->render(*snakeEngine);

    // Copia o buffer do snake para o buffer principal
    if (mainBuffer) {
        const auto* snakeBuffer = snakeRenderer->getBuffer();
        if (snakeBuffer && snakeBuffer->getBuffer()) {
            int targetWidth = snakeBuffer->getWidth();
            int targetHeight = snakeBuffer->getHeight();
            
            // Copia apenas a área necessária
            const PixelValue* srcBuffer = snakeBuffer->getBuffer();
            PixelValue* dstBuffer = mainBuffer->getBufferWritable();
            
            // Copia linha por linha para evitar problemas com padding
            for (int y = 0; y < targetHeight; y++) {
                std::memcpy(
                    dstBuffer + (y * mainBuffer->getWidth() * 4),
                    srcBuffer + (y * targetWidth * 4),
                    targetWidth * 4 * sizeof(PixelValue)
                );
            }
        }
    }
}

void Game::processWebcamBuffer(float deltaTime)
{
    if (!webcamBuffer || !videoBuffer) {
        return;
    }

    videoBuffer->clear();

    // Copia apenas uma vez o conteúdo do webcamBuffer para o videoBuffer
    const PixelValue* srcBuffer = webcamBuffer->getBuffer();
    PixelValue* dstBuffer = videoBuffer->getBufferWritable();
    
    int bufferSize = webcamBuffer->getBufferSize();
    std::memcpy(dstBuffer, srcBuffer, bufferSize * sizeof(PixelValue));

    /*
    if (webcamBuffer && videoBuffer)
    {
        videoBuffer->copyFrom(webcamBuffer.get());
    }

    videoBuffer->copyFrom(webcamBuffer.get());

    PixelValue *buffer = webcamBuffer->getBufferWritable();
    int width = webcamBuffer->getWidth();
    int height = webcamBuffer->getHeight();

    for (int i = 0; i < width * height * 4; i += 4)
    {
        buffer[i] = buffer[i];         // R
        buffer[i + 1] = buffer[i + 1]; // G
        buffer[i + 2] = buffer[i + 2]; // B
    }
    */
}

void Game::processVideoBuffer(float deltaTime)
{
    static float totalTime = 0.0f;
    totalTime += deltaTime; // Adiciona o tempo desde o último frame

    PixelValue *buffer = videoBuffer->getBufferWritable();
    int width = videoBuffer->getWidth();
    int height = videoBuffer->getHeight();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = (y * width + x) * 4;

            float wave = sin(x * 0.1f + totalTime * 2.0f) * 128 + 128;

            buffer[idx] = buffer[idx] - static_cast<PixelValue>(wave);         // R
            buffer[idx + 1] = buffer[idx + 1] - static_cast<PixelValue>(wave); // G
            buffer[idx + 2] = buffer[idx + 2] - static_cast<PixelValue>(wave); // B
        }
    }
}

void Game::setBuffers(std::unique_ptr<SceneBuffer> scene, 
                   std::unique_ptr<CanvasBuffer> video, 
                   std::unique_ptr<CanvasBuffer> webcam)
{
    sceneBuffer = std::move(scene);
    videoBuffer = std::move(video);
    webcamBuffer = std::move(webcam);
}

void Game::updatePixelsFromBuffer(const uint8_t *backupBuffer, int sourceWidth, int sourceHeight)
{

    /*

   int srcCenterX = 0;//sourceWidth / 2;
   int srcCenterY = 0;//sourceHeight / 2;

   for (int y = 0; y < height; y++) {
       for (int x = 0; x < width; x++) {
           int srcX = x + srcCenterX - width/2;
           int srcY = y + srcCenterY - height/2;

           if(srcX >= 0 && srcX < sourceWidth && srcY >= 0 && srcY < sourceHeight) {
               int srcIdx = (srcY * sourceWidth + srcX) * 4;
               pixels.setPixel(y, x,
                   sourceBuffer[srcIdx],
                   sourceBuffer[srcIdx + 1],
                   sourceBuffer[srcIdx + 2],
                   sourceBuffer[srcIdx + 3]
               );
           }
       }
   }
   */
    /*
    int srcCenterX = 0;//sourceWidth / 2;
    int srcCenterY = 0;//sourceHeight / 2;

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int srcX = x + srcCenterX - width/2;
            int srcY = y + srcCenterY - height/2;

            if(srcX >= 0 && srcX < sourceWidth && srcY >= 0 && srcY < sourceHeight) {
                int srcIdx = (srcY * sourceWidth + srcX) * 4;
                pixels.setPixel(y, x,
                    sourceBuffer[srcIdx],
                    sourceBuffer[srcIdx + 1],
                    sourceBuffer[srcIdx + 2],
                    sourceBuffer[srcIdx + 3]
                );
            }
        }
    }
    */
}