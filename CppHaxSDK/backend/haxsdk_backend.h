#pragma once

#include <cstdint>

struct Class;
struct Field;

template<typename Sig>
class Method;

struct MethodParams
{
    void* BackendMethod;
    void* Address;
    void* Thunk;
};

namespace HaxSdk
{
    template <typename T>
    T* FindStaticField();

    int32_t FindField();

    MethodParams FindMethod();

    void* NewString();

    void InitializeBackend();
}

template<typename Ret, typename... Args>
struct Method<Ret(Args...)>
{
    Method(const MethodParams& params)
    {
        BackendMethod = (Ret(*)(Args...))params.BackendMethod;
        Thunk = (Ret(*)(Args...))params.Thunk;
        Address = Original = (Ret(*)(Args...))params.Address;
    }

    Ret Invoke(void* __this, const Args&... args) const;

    Ret operator()(Args... args)
    {
        return Thunk ? Thunk(args) : Address(args);
    }

    void* BackendMethod;
    Ret(*Address)(Args...);
    Ret(*Original)(Args...);
    Ret(*Thunk)(Args...);
};