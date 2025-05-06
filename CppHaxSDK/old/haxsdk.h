#pragma once

#include <string_view>
#include <functional>

#define HAX_ASSERT(expr, msg) HaxSdk::Assert(expr, msg)

#define HAX_LOG(fmt, ...) HaxSdk::Log(std::format(fmt, ##__VA_ARGS__))
#define HAX_ERROR(m) HaxSdk::LogError(std::format(m))

struct HaxMethod;
struct HaxMethodInfo;
struct HaxFieldInfo;

enum HaxBackend
{
	HaxBackend_None = 0,
	HaxBackend_Mono = 1,
	HaxBackend_Il2cpp = 2
};

struct HaxGlobals
{
	void* m_BackendHandle;
	HaxBackend m_Backend;
    void* m_CheatModule;
    void* m_GameHWND;
    int m_MenuHotkey = 0xC0;
    bool m_MenuVisible = true;
    bool ShouldExit;
    std::vector<HaxMethodInfo> m_PreCachedMethods;
    std::vector<HaxFieldInfo> m_PreCachedFields;
    bool m_Initialized;
};

namespace HaxSdk
{
    HaxGlobals& GetGlobals();
    bool        IsMono();
    bool        IsIl2Cpp();
    void        AttachThread();
    void        Hook(void** orig, void* hook);
    void        Assert(bool expr, std::string_view message);
    void        Initialize(bool useConsole);
    void        Log(std::string_view message);
    void        LogError(std::string_view message);
    char*       ToUTF8(wchar_t* wstr, int length);
}