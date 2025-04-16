#pragma once

#define HAX_ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            MessageBoxA(GetForegroundWindow(), msg, "Hax assertion failed", MB_ICONERROR);\
            TerminateProcess(GetCurrentProcess(), 0xDEAD); \
        } \
    } while(0)

enum HaxBackend
{
	HaxBackend_None = 0,
	HaxBackend_Mono = 1,
	HaxBackend_Il2cpp = 2
};

struct HaxGlobals
{
	void* backendHandle;
	HaxBackend backend;
};

namespace HaxSdk
{
	HaxGlobals& GetGlobals();
	void Initialize();
}