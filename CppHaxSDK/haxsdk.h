#pragma once

#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR, _TEXT) if (!_EXPR) \
                                    _wassert(_CRT_WIDE(_TEXT), _CRT_WIDE(__FILE__), (unsigned)(__LINE__))
#endif

enum HaxBackend {
    HaxBackend_None = 0,
    HaxBackend_Mono = 1 << 0,
    HaxBackend_IL2CPP = 1 << 1
};

struct HaxGlobals {
    HaxBackend backend;
    void* backendHandle;
    bool visible;
    int hotkey = 0xC0;
    void* cheatModule;
};

namespace HaxSdk {
    HaxGlobals& GetGlobals();
    void InitializeCore();
}