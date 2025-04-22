#pragma once

#include <string_view>

#define HAX_ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            void* hwnd = HaxSdk::GetGlobals().GameHWND;\
            HaxSdk::LogError(msg);\
            MessageBoxA(hwnd ? (HWND)hwnd : NULL, msg, "Hax assertion failed", MB_ICONERROR);\
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
	void* BackendHandle;
	HaxBackend Backend;
    void* CheatModule;
    void* GameHWND;
    int MenuHotkey = 0xC0;
    bool MenuVisible = true;
    bool ShouldExit;
};

namespace HaxSdk
{
    HaxGlobals& GetGlobals();
    void Initialize(bool useConsole);
    void Log(std::string_view message);
    void LogError(std::string_view message);
}