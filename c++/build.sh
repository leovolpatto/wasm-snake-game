#!/bin/bash

# Diretório base
BASE_DIR="src"

# Lista de todos os arquivos fonte
SOURCE_FILES=(
    "$BASE_DIR/main.cpp"
    "$BASE_DIR/js/helpers.cpp"
    "$BASE_DIR/scene/sceneBuffer.cpp"
    "$BASE_DIR/scene/sceneObject.cpp"
    "$BASE_DIR/wasm/canvasBuffer.cpp"
    "$BASE_DIR/wasm/bindings.cpp"
    "$BASE_DIR/movement/circular.cpp"
    "$BASE_DIR/movement/linear.cpp"
    "$BASE_DIR/objects/game.cpp"
)

# Diretório de saída (TypeScript)
OUTPUT_DIR="../typescript/public"
OUTPUT_FILE="snake.js"

EXPORTED_FUNCTIONS=(
    "_main"
    "_malloc"
    "_free"
    "_init"
    "_freeBuffer"
    "_initialize"
    "_processBuffer"
    "_getBufferPointer"
    "_initializeBuffer"
    "_applySnakeHeadEffect"
    "_allocateMemoryForWebcam"
    "_initVideoBuffers"
    "_initSceneBuffer"
    "_getSceneBufferInfo"
    "_getWebcamBufferInfo"
    "_getVideoBufferInfo"
    "_processSceneBuffer"
    "_processWebcamBuffer"
    "_processVideoBuffer"
    "_processKeyPressed"
    "_cleanupBuffers"     
)

# Converte array de funções para string
FUNCTIONS_STRING="["
for func in "${EXPORTED_FUNCTIONS[@]}"
do
    FUNCTIONS_STRING+="'$func',"
done
FUNCTIONS_STRING="${FUNCTIONS_STRING%,}]"

# clang-tidy src/*.cpp -- \
#   -Iinclude \
#   -I/home/leo/leo/technical-ecstasy/emsdk/upstream/emscripten/cache/sysroot/include -header-filter=.*

emcc ${SOURCE_FILES[@]} \
    -I./include \
    -o $OUTPUT_DIR/$OUTPUT_FILE \
    -gsource-map \
    -s WASM=1 \
    -s EXPORTED_FUNCTIONS="$FUNCTIONS_STRING" \
    -s EXPORTED_RUNTIME_METHODS="['ccall','cwrap']" \
    -s ENVIRONMENT='web' \
    -O3

# Verifica se a compilação foi bem sucedida
if [ $? -eq 0 ]; then
    echo "Compilação concluída com sucesso!"
    echo "Arquivos gerados em: $OUTPUT_DIR"
else
    echo "Erro durante a compilação!"
    exit 1
fi