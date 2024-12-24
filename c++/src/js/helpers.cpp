#include <emscripten.h>
#include "js/helpers.hpp"

extern "C" {
    EM_JS(void, consoleLog, (const char* str), {
        console.log('[FROM C++]' + UTF8ToString(str));
    });
}