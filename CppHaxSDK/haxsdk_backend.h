#pragma once

#include <cstdint>
#include <stdexcept>


struct HaxClass;
template<typename T>
struct HaxMethod;

namespace HaxSdk
{
    void AttachThread();
    void InitializeBackend();
}

struct HaxType;

struct ManagedMethod
{
    ManagedMethod(void* backendMethod) : m_NativePtr(backendMethod), Address(nullptr), Thunk(nullptr) {}

    void* Invoke(void* __this, void** args);
    void Hook(void** addr, void* hookFunc);

    void* Address;
    void* Thunk;

private:
    void* m_NativePtr;
};

struct HaxAssembly
{
    static bool Exists(const char* assembly, HaxAssembly* out = nullptr);
    static HaxAssembly Find(const char* assembly);
    HaxClass FindClass(const char* nameSpace, const char* klass);
    const char* GetName();
};

struct HaxClass 
{
    HaxClass(void* nativePtr) : m_NativePtr(nativePtr) {}

    operator void* () { return m_NativePtr; }

    static HaxClass     Find(const char* assembly, const char* nameSpace, const char* klass);
    int32_t             FindField(const char* field);
    void*               FindStaticField(const char* field);
    ManagedMethod       FindMethod(const char* name, const char* signature = nullptr);
    const char*         GetName();
    const char*         GetNamespace();
    //void*               FindStaticField(const char* field);
    //ManagedMethod       FindMethod(const char* method);
    //HaxType*            GetType();

private:
    void* m_NativePtr; // Il2cppClass* or MonoClass*
};

struct HaxObject 
{
    HaxClass GetClass();
    void* Unbox();
};

template<typename Ret, typename... Args>
struct HaxMethod<Ret(Args...)>
{

    HaxMethod() : HaxMethod(ManagedMethod(nullptr)) {}

    HaxMethod(ManagedMethod managed) : m_ManagedMethod(managed)
    {
        Address = HookOrig = (Ret(*)(Args...))(managed.Address);
        m_Thunk = (Ret(__stdcall*)(Args..., void**))(managed.Thunk);
    }

    void Hook(void* hookFunc)
    {
        m_ManagedMethod.Hook((void**)&HookOrig, hookFunc);
    }

    Ret Thunk(const Args&... args) 
    {
        HAX_ASSERT(m_Thunk, "Thunk is not available for your game");
        void* exc = nullptr;
        if constexpr (std::is_void_v<Ret>)
        {
            m_Thunk(args..., &exc);
            if (exc)
                throw std::runtime_error("Thunk failed");
            return;
        }
        else 
        {
            Ret ret = m_Thunk(args..., &exc);
            if (exc)
                throw std::runtime_error("Thunk failed");
            return ret;
        }
    };

    bool ThunkAvailable() { return m_Thunk != nullptr; }

    Ret Invoke(const Args&... args)
    {
        static_assert(sizeof... (args) > 0);

        void* packedArgs[] = { Pack(args)... };
        int nArgs = sizeof(packedArgs) / sizeof(void*);
        HaxObject obj = m_ManagedMethod.Invoke(packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1);

        if constexpr (std::is_void_v<Ret>) return;
        else if constexpr (std::is_base_of_v<HaxObject, Ret>) return obj.as<Ret>();
        else return *(Ret*)(obj.Unbox());
    }

    Ret InvokeStatic(const Args&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            HaxObject res(m_ManagedMethod.Invoke(nullptr, nullptr));

            if constexpr (std::is_void_v<Ret>) return;
            else if constexpr (std::is_base_of_v<HaxObject, Ret>) return res.as<Ret>();
            else return *(Ret*)(res.Unbox());
        }
        else
        {
            void* packedArgs[] = { Pack(args)... };
            HaxObject res = m_ManagedMethod.Invoke(nullptr, packedArgs);

            if constexpr (std::is_void_v<Ret>) return;
            else if constexpr (std::is_base_of_v<HaxObject, Ret>) return res.as<Ret>();
            else return *(Ret*)(res.Unbox());
        }
    }

private:
    template <typename T>
    void* Pack(const T& val)
    {
        if constexpr (std::is_pointer_v<T> || std::is_base_of_v<HaxObject, T>)
        {
            return (void*)(val);
        }
        else
        {
            return (void*)(&val);
        }
    }

public:
    Ret(*Address)(Args...);
    Ret(*HookOrig)(Args...);

private:
    Ret(__stdcall* m_Thunk)(Args..., void**);
    ManagedMethod m_ManagedMethod;
};