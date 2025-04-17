#pragma once

#include <string_view>

#include "backend/haxsdk_il2cpp.h"
#include "backend/haxsdk_mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "third_party/detours/x64/detours.h"

#define HAX_ASSERT(expr, msg) \
    do { \
        if (!(expr)) { \
            void* hwnd = HaxSdk::GetGlobals().GameHWND;\
            HaxSdk::Log(msg);\
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

template<typename Sig>
class Method;

namespace HaxSdk
{
	HaxGlobals& GetGlobals();
	void Initialize(bool useConsole);
    void Log(std::string_view message);

    inline void AttachThread()
    {
        HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? Mono::AttachThread() : Il2Cpp::AttachThread();
    }
    
    inline bool AssemblyExists(const char* assembly)
    {
        return HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? Mono::AssemblyExists(assembly, nullptr) : Il2Cpp::AssemblyExists(assembly, nullptr);
    }

    inline int32_t FindField(const char* assembly, const char* nameSpace, const char* klass, const char* field)
    {
        return HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? Mono::FindField(assembly, nameSpace, klass, field) : Il2Cpp::FindField(assembly, nameSpace, klass, field);
    }

    template <typename T>
    inline T* FindStaticField(const char* assembly, const char* nameSpace, const char* klass, const char* field)
    {
        return (T*)(HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? Mono::FindStaticField(assembly, nameSpace, klass, field) : Il2Cpp::FindStaticField(assembly, nameSpace, klass, field));
    }

    inline MethodParams FindMethod(const char* assembly, const char* nameSpace, const char* klass, const char* method, const char* sig = nullptr)
    {
        return HaxSdk::GetGlobals().Backend == HaxBackend_Mono ? Mono::FindMethod(assembly, nameSpace, klass, method, sig) : Il2Cpp::FindMethod(assembly, nameSpace, klass, method, sig);
    }
}

template<typename Ret, typename... Args>
struct Method<Ret(Args...)>
{
    Method() = default;

    Method(const MethodParams& params)
    {
        m_BackendMethod = params.BackendMethod;
        Thunk = (Ret(__stdcall*)(Args...,MonoException**))params.Thunk;
        Address = HookOrig = (Ret(*)(Args...))params.Address;
    }

private:
    template <typename T>
    void* Pack(const T& val)
    {
        if constexpr (std::is_pointer_v<T>)
        {
            return (void*)(val);
        }
        else
        {
            return (void*)(&val);
        }
    }

public:
    void Hook(void* hookFunc)
    {
        DetourAttach((void**)&HookOrig, hookFunc);
    }

    Ret Invoke(const Args&... args) const
    {
        void* packedArgs[] = { Pack(args)... };
        int nArgs = sizeof(packedArgs) / sizeof(void*);
        MonoObject* res = mono_runtime_invoke(m_BackendMethod, packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1, nullptr);
        if constexpr (std::is_void_v<Ret>) return;
        else if constexpr (std::is_pointer_v<Ret>) return (Ret)res;
        else return *(Ret*)(mono_object_unbox(res));
    }

    Ret InvokeStatic(const Args&... args) const
    {
        void* res;
        if constexpr (sizeof...(args) == 0)
        {
            res = mono_runtime_invoke(m_BackendMethod, nullptr, nullptr, nullptr);
        }
        else
        {
            void* packedArgs[] = { Pack(args)... };
            res = mono_runtime_invoke(m_BackendMethod, nullptr, packedArgs, nullptr);
        }

        if constexpr (std::is_void_v<Ret>) return;
        if constexpr (std::is_pointer_v<Ret>) return res;
        return *(Ret*)mono_object_unbox(res);
    }

    Ret(*Address)(Args...);
    Ret(*HookOrig)(Args...);
    Ret(__stdcall *Thunk)(Args...,MonoException**);
private:
    void* m_BackendMethod;
};