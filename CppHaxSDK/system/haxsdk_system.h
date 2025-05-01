#pragma once

#include <iostream>
#include <type_traits>

#include "../haxsdk.h"
#include "../backend/haxsdk_mono.h"
#include "../backend/haxsdk_il2cpp.h"

template<typename T>
struct HaxMethod;

template <typename T>
struct NativeArray
{
    void* vtable;
    void* monitor;
    void* bounds;
    size_t length;
    __declspec(align(8)) T vector[1];
};

static void ThrowNullRef();

namespace System
{
    using Int32 = int32_t;

    struct Assembly;
    struct AssemblyName;
    template <typename T>
    struct Array;
    struct Type;
    struct FieldInfo;
    struct MethodInfo;
    struct Object;
    struct NullReferenceException;
}

namespace System 
{
    /*struct ManagedObject
    {
        friend System::Object;

        ManagedObject(void* ptr) : m_ManagedPtr(ptr) {}

        inline operator bool() const { return m_ManagedPtr != nullptr; }
        inline bool operator==(const ManagedObject& rhs) const { return rhs.m_ManagedPtr == m_ManagedPtr; }

        inline operator void* () const
        {
            if (!m_ManagedPtr)
                ThrowNullRef();
            return m_ManagedPtr;
        }

        inline operator char* () const
        {
            if (!m_ManagedPtr)
                ThrowNullRef();
            return (char*)m_ManagedPtr;
        }

    private:
        void* m_ManagedPtr;
    };*/

    struct Object
    {
        inline bool operator==(const Object& o) const { return ManagedPtr == o.ManagedPtr; }

        static inline bool      Equals(const Object& objA, const Object& objB) { return objA.ManagedPtr == objB.ManagedPtr; }
        inline bool             IsNull() const { return ManagedPtr == nullptr; }

        static Type             TypeOf();

        bool                    Equals(const Object& o) const;
        Type                    GetType();
        void*                   Unbox();

        union 
        {
            void*               ManagedPtr;
            MonoObject*         MonoObj;
            Il2CppObject*       Il2CppObj;
            MonoString*         MonoStr;
            Il2CppString*       Il2cppStr;
            MonoArray<void*>*          MonoArr;
            Il2CppArray<void*>*        Il2CppArr;
            MonoException*      MonoEx;
            Il2CppException*    Il2CppEx;
        };
    };

    struct Exception : Object
    {
        const char* GetMessageText() const;
        const char* GetStackTrace() const;
    };

    struct NullReferenceException : Exception
    {
        static inline Type TypeOf() { return Assembly::GetMscorlib().GetType("System", "NullReferenceException"); }

        static NullReferenceException New();
    };

    struct ArgumentOutOfRangeException : Exception
    {
        static inline Type TypeOf() { return Assembly::GetMscorlib().GetType("System", "ArgumentOutOfRangeException"); }

        static ArgumentOutOfRangeException New();
    };

    struct String : Object
    {
        static String New(const char* text);
        static Type TypeOf();

        friend std::wostream& operator<<(std::wostream& os, const String& str);
        friend std::ostream& operator<<(std::ostream& os, const String& str);
    };

    template <typename T>
    struct Array // : Object
    {
        inline T* begin()                           { return NativeArr.vector; }
        inline const T* begin() const               { return NativeArr.vector; }
        inline T* end()                             { return &NativeArr.vector[length]; }
        inline const T* end() const                 { return &NativeArr.vector[length]; }

        inline T& operator[](size_t i)              { return NativeArr.vector[i]; }
        inline const T& operator[](size_t i) const  { return NativeArr.vector[i]; }

        NativeArray<T> NativeArr;
    };

    struct AppDomain
    {
    private:
        AppDomain(MonoDomain* monoDomain) : m_MonoDomain(monoDomain) {}
        AppDomain(Il2CppDomain* il2cppDomain) : m_Il2CppDomain(il2cppDomain) {}

    public:
        inline operator void*() { return m_NativePtr; }

        static AppDomain        GetDefaultDomain();

        Assembly                Load(const char* name);

    private:
        union
        {
            void* m_NativePtr;
            MonoDomain* m_MonoDomain;
            Il2CppDomain* m_Il2CppDomain;
        };
    };

    struct Assembly
    {
        Assembly(MonoImage* monoImage) : m_MonoImage(monoImage) {}
        Assembly(Il2CppImage* il2cppImage) : m_Il2CppImage(il2cppImage) {}

        static Assembly GetMscorlib();
        static Assembly GetUnityCoreLib();

        AssemblyName GetName();
        Type GetType(const char* nameSpace, const char* name);

    private:
        union
        {
            void* m_NativePtr;
            MonoImage* m_MonoImage;
            Il2CppImage* m_Il2CppImage;
        };
    };

    struct AssemblyName
    {
        AssemblyName(MonoAssemblyName* monoAssemblyName) : m_MonoAssemblyName(monoAssemblyName) {}
        AssemblyName(Il2CppAssemblyName* il2cppAssemblyName) : m_Il2CppAssemblyName(il2cppAssemblyName) {}

        const char* GetName();

    private:
        union
        {
            void* m_NativePtr;
            MonoAssemblyName* m_MonoAssemblyName;
            Il2CppAssemblyName* m_Il2CppAssemblyName;
        };
    };

    struct Type
    {
        Type(MonoReflectionType* monoType) : m_MonoReflectionType(monoType) {}
        Type(Il2CppReflectionType* il2cppType) : m_Il2CppReflectionType(il2cppType) {}

        FieldInfo GetField(const char* name);
        MethodInfo GetMethod(const char* name, const char* sig = nullptr);

    private:
        union
        {
            void* m_NativePtr;
            MonoReflectionType* m_MonoReflectionType;
            Il2CppReflectionType* m_Il2CppReflectionType;
        };
    };

    struct FieldInfo
    {
        friend struct Type;

        operator void* () { return m_NativePtr; }

        int32_t GetFieldOffset();
        void* GetStaticAddress();

    private:
        FieldInfo();
        FieldInfo(MonoClassField* monoClassField) : m_MonoClassField(monoClassField) {}
        FieldInfo(Il2CppFieldInfo* il2cppFieldInfo) : m_Il2CppFieldInfo(il2cppFieldInfo) {}

        union
        {
            void* m_NativePtr;
            MonoClassField* m_MonoClassField;
            Il2CppFieldInfo* m_Il2CppFieldInfo;
        };
    };

    struct MethodInfo
    {
        friend struct Type;

        MethodInfo() = default;
        MethodInfo(void* nativePtr) : m_NativePtr(nativePtr) {}
        MethodInfo(MonoMethod* monoMethod) : m_MonoMethod(monoMethod) {}
        MethodInfo(Il2CppMethodInfo* il2cppMethodInfo) : m_Il2CppMethodInfo(il2cppMethodInfo) {}

        operator void* () { return m_NativePtr; }

        Object Invoke(void* __this, void** args);
        void Hook(void** ptr, void* func);
        void* GetUnmanagedThunk();
        void* GetFunctionPointer();

    private:
        union
        {
            void* m_NativePtr;
            MonoMethod* m_MonoMethod;
            Il2CppMethodInfo* m_Il2CppMethodInfo;
        };
    };
}

//template<typename Ret, typename... Args>
//struct HaxMethod<Ret(Args...)>
//{
//    HaxMethod() : m_MethodInfo((void*)nullptr) {}
//    HaxMethod(System::MethodInfo methodInfo) : m_MethodInfo(methodInfo) 
//    {
//        m_Address = HookOrig = (Ret(*)(Args...))(methodInfo.GetFunctionPointer());
//        m_Thunk = (Ret(__stdcall*)(Args...,MonoException**))(methodInfo.GetUnmanagedThunk());
//    }
//
//    void Hook(void* hookFunc)
//    {
//        m_MethodInfo.Hook((void**)&HookOrig, hookFunc);
//    }
//
//    Ret Thunk(const Args&... args)
//    {
//        HAX_ASSERT(m_Thunk, "Thunk is not available for your game");
//        MonoException* exc = nullptr;
//        if constexpr (std::is_void_v<Ret>)
//        {
//            m_Thunk(args..., &exc);
//            if (exc)
//                throw System::Exception(exc);
//            return;
//        }
//        else
//        {
//            Ret ret = m_Thunk(args..., &exc);
//            if (exc)
//                throw System::Exception(exc);
//            return ret;
//        }
//    };
//
//    Ret Address(const Args&... args)
//    {
//        try
//        {
//            if constexpr (std::is_void_v<Ret>) m_Address(args...);
//            else return m_Address(args...);
//        }
//        catch (Il2CppExceptionWrapper& ex)
//        {
//            throw System::Exception(ex);
//        }
//    }
//
//    bool ThunkAvailable() { return m_Thunk != nullptr; }
//
//    Ret Invoke(const Args&... args)
//    {
//        static_assert(sizeof... (args) > 0);
//
//        void* packedArgs[] = { Pack(args)... };
//        int nArgs = sizeof(packedArgs) / sizeof(void*);
//        System::Object obj = m_MethodInfo.Invoke(packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1);
//
//        if constexpr (std::is_void_v<Ret>) return;
//        else if constexpr (std::is_base_of_v<System::Object, Ret>) return obj.as<Ret>();
//        else return *(Ret*)(obj.Unbox());
//    }
//
//    Ret InvokeStatic(const Args&... args)
//    {
//        if constexpr (sizeof...(args) == 0)
//        {
//            System::Object res(m_MethodInfo.Invoke(nullptr, nullptr));
//
//            if constexpr (std::is_void_v<Ret>) 
//                return;
//            else if constexpr (std::is_base_of_v<System::Object, Ret>) 
//                return res.as<Ret>();
//            else 
//                return *(Ret*)res.Unbox();
//        }
//        else
//        {
//            void* packedArgs[] = { Pack(args)... };
//            System::Object res = m_MethodInfo.Invoke(nullptr, packedArgs);
//
//            if constexpr (std::is_void_v<Ret>) 
//                return;
//            else if constexpr (std::is_base_of_v<System::Object, Ret>) 
//                return res.as<Ret>();
//            else 
//                return *(Ret*)res.Unbox();
//        }
//    }
//
//private:
//    template <typename T>
//    void* Pack(const T& val)
//    {
//        if constexpr (std::is_pointer_v<T>) return val;
//        if constexpr (std::is_base_of_v<System::Object, T>) return val.Ptr();
//        else return &val;
//    }
//
//public:
//    Ret(*HookOrig)(Args...) = nullptr;
//
//private:
//    Ret(*m_Address)(Args...) = nullptr;
//    Ret(__stdcall* m_Thunk)(Args...,MonoException**) = nullptr;
//    System::MethodInfo m_MethodInfo;
//};
//
//static void ThrowNullRef() { throw System::NullReferenceException::New(); }