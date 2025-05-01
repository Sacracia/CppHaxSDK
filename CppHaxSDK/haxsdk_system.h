#pragma once

#include <type_traits>

#include "haxsdk.h"
#include "backend/haxsdk_mono.h"
#include "backend/haxsdk_il2cpp.h"

#define ASSERT_NOT_NULL(x) if (!x) throw NullReferenceException()::New()

using Int32 = int32_t;
using Char = wchar_t;

namespace System
{
    struct Assembly;
    struct AssemblyName;
    struct Type;
    struct FieldInfo;
    struct MethodInfo;
    struct Object;
    struct NullReferenceException;
    struct String;
    struct ValueType;
}

template <typename T>
System::Type typeof();

template <>
extern System::Type typeof<int>();

struct HaxMethod;
struct HaxMethodInfo;
struct HaxFieldInfo;

struct UnityObject
{
    void* __space[2]; // System.Object
    void* m_CachedPtr;
};

template <typename T>
struct NativeList;

namespace System
{
    struct ValueType
    {

    };

    struct Object
    {
        Object(void* ptr) : m_ManagedPtr(ptr) {}

        inline bool operator==(const Object& o) const { return m_ManagedPtr == o.m_ManagedPtr; }

        static inline bool      Equals(const Object& objA, const Object& objB) { return objA.m_ManagedPtr == objB.m_ManagedPtr; }
        inline bool             IsNull() const { return m_ManagedPtr == nullptr; }

        static Type             TypeOf();
        static Object           Box(Type type, void* value);

        bool                    Equals(const Object& o) const;
        Type                    GetType() const;
        void*                   Unbox() const;

        union 
        {
            void*                   m_ManagedPtr;
            MonoObject*             m_MonoObj;
            Il2CppObject*           m_Il2CppObj;
            MonoString*             m_MonoStr;
            Il2CppString*           m_Il2cppStr;
            void*                   m_MonoArr;
            void*                   m_Il2CppArr;
            MonoException*          m_MonoEx;
            Il2CppException*        m_Il2CppEx;
            MonoReflectionType*     m_MonoType;
            Il2CppReflectionType*   m_Il2CppType;
            UnityObject*            m_UnityObject;
        };
    };

    struct Type : Object
    {


        Object CreateInstance();
        FieldInfo GetField(const char* name, bool doAssert = false);
        MethodInfo GetMethod(const char* name, const char* sig = nullptr, bool doAssert = false);
    };

    struct String : Object
    {
        String(const Object& o) : Object(o) {}

        inline String operator+(const String& s) { return String::Concat(*this, s); }

        static Type TypeOf();
        static String New(const char* data);
        static String Concat(const String& s1, const String& s2);

        Int32 CompareTo(const String& s);

        wchar_t* GetRawStringData();
        Int32 Length();
    };

    // System.Reflection
    struct AppDomain
    {
        AppDomain(void* ptr) : m_NativePtr(ptr) {}

        static AppDomain        GetDefaultDomain();

        Assembly                Load(const char* name, bool doAssert = false);

        union
        {
            void* m_NativePtr;
            MonoDomain* m_MonoDomain;
            Il2CppDomain* m_Il2CppDomain;
        };
    };

    struct Assembly
    {
        Assembly(void* ptr) : m_NativePtr(ptr) {}

        inline operator bool() { return m_NativePtr != nullptr; }

        static Assembly GetMscorlib();
        static Assembly GetUnityCoreLib();

        AssemblyName GetName();
        Type GetType(const char* nameSpace, const char* name, bool doAssert = false);

        union
        {
            void* m_NativePtr;
            MonoImage* m_MonoImage;
            Il2CppImage* m_Il2CppImage;
        };
    };

    struct AssemblyName
    {
        AssemblyName(void* ptr) : m_NativePtr(ptr) {}

        const char* GetName();

        union
        {
            void* m_NativePtr;
            MonoAssemblyName* m_MonoAssemblyName;
            Il2CppAssemblyName* m_Il2CppAssemblyName;
        };
    };

    struct FieldInfo
    {
        FieldInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name) : m_NativePtr(nullptr)
        {
            auto& globals = HaxSdk::GetGlobals();
            if (!globals.m_Initialized)
                globals.m_PreCachedFields.emplace_back(*this, assembly, nameSpace, klass, name);
        }

        FieldInfo(void* ptr) : m_NativePtr(ptr) {}

        int32_t GetFieldOffset();
        void* GetStaticAddress();
        void GetStaticValue(void* value);

        union
        {
            void* m_NativePtr;
            MonoClassField* m_MonoClassField;
            Il2CppFieldInfo* m_Il2CppFieldInfo;
        };
    };

    struct MethodInfo
    {
        MethodInfo(void* ptr) : m_NativePtr(ptr) {}

        Object Invoke(void* __this, void** args);
        void* GetUnmanagedThunk() const;
        void* GetFunctionPointer() const;

        union
        {
            void* m_NativePtr;
            MonoMethod* m_MonoMethod;
            Il2CppMethodInfo* m_Il2CppMethodInfo;
        };
    };

    // Exceptions
    struct Exception : Object
    {
        Exception(void* ptr) : Object(ptr) {}

        const char* GetMessageText() const;
        const char* GetStackTrace() const;
    };

    struct NullReferenceException : Exception
    {
        NullReferenceException(void* ptr) : Exception(ptr) {}

        static inline Type TypeOf() { return Assembly::GetMscorlib().GetType("System", "NullReferenceException"); }

        static NullReferenceException New();
    };

    struct ArgumentOutOfRangeException : Exception
    {
        static inline Type TypeOf() { return Assembly::GetMscorlib().GetType("System", "ArgumentOutOfRangeException"); }

        static ArgumentOutOfRangeException New();
    };

    // Containers
    template <typename T>
    struct Array : Object
    {
                    Array(const System::Object& o) : Object(o) {}

        T&          operator[](size_t i);
        const T&    operator[](size_t i) const;

        T*          begin();
        const T*    begin() const;
        T*          end();
        const T*    end() const;

        static Type TypeOf() { return Assembly::GetMscorlib().GetType("System", "Array"); }
        static void Clear(Array<T> array, Int32 index, Int32 length);
        static void Sort(Array<T> array, Int32 index, Int32 length);
    };

    template <typename T>
    struct List : Object
    {
                    List(const System::Object& o) : Object(o) {}

        T&          operator[](size_t i);
        const T&    operator[](size_t i) const;

        T*          begin();
        const T*    begin() const;
        T*          end();
        const T*    end() const;

        Int32       Length() const;
        bool        Contains(T val) const;
        void        Add(T val) const;
        void        Clear() const;
    };
}

struct Int32_B : System::Object
{
    int Unbox() { if (!m_ManagedPtr) throw System::NullReferenceException::New(); return *(int*)((char*)m_ManagedPtr + sizeof(MonoObject)); }
};

inline Int32_B Box(int v)
{
    return HaxSdk::IsMono() ? Int32_B(Mono::BoxValue(typeof<int>().m_MonoType, &v)) : Int32_B(Il2Cpp::BoxValue(typeof<int>().m_Il2CppType, &v));
}

struct HaxMethodInfo
{
    HaxMethodInfo(HaxMethod& method, const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* sig)
        : m_Method(method), m_Assembly(assembly), m_Namespace(nameSpace), m_Class(klass), m_Name(name), m_Signature(sig) {}

    HaxMethod& m_Method;
    const char* m_Assembly;
    const char* m_Namespace;
    const char* m_Class;
    const char* m_Name;
    const char* m_Signature;
};

struct HaxFieldInfo
{
    HaxFieldInfo(System::FieldInfo& method, const char* assembly, const char* nameSpace, const char* klass, const char* name)
        : m_Field(method), m_Assembly(assembly), m_Namespace(nameSpace), m_Class(klass), m_Name(name) {}

    System::FieldInfo& m_Field;
    const char* m_Assembly;
    const char* m_Namespace;
    const char* m_Class;
    const char* m_Name;
};

struct HaxMethod
{
    HaxMethod(const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* sig = nullptr) : m_MethodInfo(nullptr)
    { 
        HaxGlobals& globals = HaxSdk::GetGlobals();
        if (!globals.m_Initialized)
            globals.m_PreCachedMethods.emplace_back(*this, assembly, nameSpace, klass, name, sig);
    }

    HaxMethod(const System::MethodInfo& methodInfo) : m_MethodInfo(methodInfo)
    {
        m_Address = m_HookOrig = methodInfo.GetFunctionPointer();
        m_Thunk = methodInfo.GetUnmanagedThunk();
    }

    void Hook(void* hookFunc)
    {
        HAX_ASSERT(m_HookOrig, "Method not initialized");
        HaxSdk::Hook((void**)&m_HookOrig, hookFunc);
    }

    template<typename Ret, typename... Args>
    Ret Thunk(const Args&... args) const
    {
        HAX_ASSERT(m_Thunk, "Thunk is not available");
        MonoException* exc = nullptr;
        if constexpr (std::is_void_v<Ret>)
        {
            ((void(__stdcall*)(Args..., MonoException**))m_Thunk)(args..., &exc);
            if (exc)
                throw System::Exception(exc);
            return;
        }
        else if constexpr (std::is_base_of_v<System::Object, Ret>)
        {
            Ret ret(System::Object((((void*(__stdcall*)(Args..., MonoException**))m_Thunk)(args..., &exc))));
            if (exc)
                throw System::Exception(exc);
            return ret;
        }
        else if constexpr ((std::is_base_of_v<System::ValueType, Ret>))
        {
            System::Object o = ((void*(__stdcall*)(Args..., MonoException**))m_Thunk)(args..., &exc);
            if (exc)
                throw System::Exception(exc);
            return *(Ret*)o.Unbox();
        }
        else
        {
            Ret ret = ((Ret(__stdcall*)(Args..., MonoException**))m_Thunk)(args..., &exc);
            if (exc)
                throw System::Exception(exc);
            return ret;
        }
    };

    template<typename Ret, typename... Args>
    Ret Address(const Args&... args)
    {
        try
        {
            using Fn = Ret(*)(Args...);
            if constexpr (std::is_void_v<Ret>) ((Fn)m_Address)(args...);
            else return ((Fn)m_Address)(args...);
        }
        catch (Il2CppExceptionWrapper& ex)
        {
            throw System::Exception(ex.ex);
        }
    }

    template<typename Ret, typename... Args>
    Ret Invoke(const Args&... args)
    {
        static_assert(sizeof... (args) > 0);

        void* packedArgs[] = { Pack(args)... };
        int nArgs = sizeof(packedArgs) / sizeof(void*);
        System::Object obj = m_MethodInfo.Invoke(packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1);

        if constexpr (std::is_void_v<Ret>) return;
        else if constexpr (std::is_base_of_v<System::Object, Ret>) return Ret(obj.m_ManagedPtr);
        else return *(Ret*)(obj.Unbox());
    }

    template<typename Ret, typename... Args>
    Ret InvokeStatic(const Args&... args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            System::Object res(m_MethodInfo.Invoke(nullptr, nullptr));

            if constexpr (std::is_void_v<Ret>)
                return;
            else if constexpr (std::is_base_of_v<System::Object, Ret>)
                return Ret(res.m_ManagedPtr);
            else
                return *(Ret*)res.Unbox();
        }
        else
        {
            void* packedArgs[] = { Pack(args)... };
            System::Object res = m_MethodInfo.Invoke(nullptr, packedArgs);

            if constexpr (std::is_void_v<Ret>)
                return;
            else if constexpr (std::is_base_of_v<System::Object, Ret>)
                return Ret(res.m_ManagedPtr);
            else
                return *(Ret*)res.Unbox();
        }
    }

    template <typename T>
    void* Pack(const T& val)
    {
        if constexpr (std::is_pointer_v<T>) return val;
        if constexpr (std::is_base_of_v<System::Object, T>) return val.m_ManagedPtr;
        else return (void*)&val;
    }

    System::MethodInfo m_MethodInfo;
    void* m_HookOrig = nullptr;
    void* m_Address = nullptr;
    void* m_Thunk = nullptr;
};

template <typename T>
struct NativeList
{
    void* __space[2];
    System::Array<T> arr;
    int32_t length;
    int32_t version;
    void* pSyncRoot;
};

template <typename T>
T* System::Array<T>::begin()
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();
    MonoArray<T>* arr = ((MonoArray<T>*)m_MonoArr);
    return arr->vector;
}

template <typename T>
const T* System::Array<T>::begin() const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();
    MonoArray<T>* arr = ((MonoArray<T>*)m_MonoArr);
    return arr->vector;
}


template <typename T>
T* System::Array<T>::end()
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();
    MonoArray<T>* arr = ((MonoArray<T>*)m_MonoArr);
    return &arr->vector[arr->max_length];
}

template <typename T>
const T* System::Array<T>::end() const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();
    MonoArray<T>* arr = ((MonoArray<T>*)m_MonoArr);
    return &arr->vector[arr->max_length];
}

template <typename T>
T& System::Array<T>::operator[](size_t i)
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    MonoArray<T>* arr = ((MonoArray<T>*)m_MonoArr);

    if (i < 0 || i >= arr->max_length)
        throw ArgumentOutOfRangeException::New();

    return arr->vector[i];
}


template <typename T>
const T& System::Array<T>::operator[](size_t i) const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    MonoArray<T>* arr = ((MonoArray<T>*)m_MonoArr);

    if (i < 0 || i >= arr->max_length)
        throw ArgumentOutOfRangeException::New();

    return arr->vector[i];
}

template <typename T>
void System::Array<T>::Clear(Array<T> array, Int32 index, Int32 length)
{
    static HaxMethod method = TypeOf().GetMethod("Clear", "System.Void(System.Array,System.Int32,System.Int32");

    if (HaxSdk::IsMono())
    {
        if (method.m_Thunk) { method.Thunk<Array<T>, Int32, Int32>(array, index, length); }
        else { method.InvokeStatic<Array<T>, Int32, Int32>(array, index, length); }
        return;
    }

    method.Address<Array<T>, Int32, Int32>(array, index, length);
}

template <typename T>
void System::Array<T>::Sort(Array<T> array, Int32 index, Int32 length)
{
    static HaxMethod method = TypeOf().GetMethod("Sort", "System.Void(System.Array,System.Int32,System.Int32)");

    if (HaxSdk::IsMono())
    {
        if (method.m_Thunk) { m_Thunk<void, Array<T>, Int32, Int32>(array, index, length); }
        else { method.InvokeStatic<void(Array<T>, Int32, Int32)>(array); }
        return;
    }

    method.Address(array, index, length);
}

template <typename T>
T& System::List<T>::operator[](size_t i)
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    auto native = (NativeList<T>*)m_ManagedPtr;
    if (i < 0 || i >= native->length)
        throw ArgumentOutOfRangeException::New();

    return native->arr[i];
}

template <typename T>
const T& System::List<T>::operator[](size_t i) const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    auto native = (NativeList<T>*)m_ManagedPtr;
    if (i < 0 || i >= native->length)
        throw ArgumentOutOfRangeException::New();

    return native->arr[i];
}

template <typename T>
T* System::List<T>::begin()
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    return ((NativeList<T>*)m_ManagedPtr)->arr.begin();
}

template <typename T>
const T* System::List<T>::begin() const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    return ((NativeList<T>*)m_ManagedPtr)->arr.begin();
}

template <typename T>
T* System::List<T>::end()
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    auto native = (NativeList<T>*)m_ManagedPtr;
    return native->arr.begin() + native->length;
}

template <typename T>
const T* System::List<T>::end() const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    auto native = (NativeList<T>*)m_ManagedPtr;
    return native->arr.begin() + native->length;
}

template <typename T>
Int32 System::List<T>::Length() const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    return ((NativeList<T>*)m_ManagedPtr)->length;
}

template <typename T>
bool System::List<T>::Contains(T val) const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    static HaxMethod method = this->GetType().GetMethod("Contains", nullptr, true);

    if (method.m_Thunk)
        return method.Thunk<bool, void*, T>(m_ManagedPtr, val);

    return HaxSdk::IsMono() ? method.Invoke<bool, void*, T>(m_ManagedPtr, val) : method.Address<bool, void*, T>(m_ManagedPtr, val);
}

template <typename T>
void System::List<T>::Add(T val) const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    static HaxMethod method = this->GetType().GetMethod("Add1", nullptr, true);

    if (method.m_Thunk)
    {
        method.Thunk<bool, void*, T>(m_ManagedPtr, val);
        return;
    }

    HaxSdk::IsMono() ? method.Invoke<bool, void*, T>(m_ManagedPtr, val) : method.Address<bool, void*, T>(m_ManagedPtr, val);
}

template <typename T>
void System::List<T>::Clear() const
{
    if (!m_ManagedPtr)
        throw NullReferenceException::New();

    static HaxMethod method = this->GetType().GetMethod("Clear", nullptr, true);

    if (method.m_Thunk)
    {
        method.Thunk<void, void*>(m_ManagedPtr);
        return;
    }

    HaxSdk::IsMono() ? method.Invoke<void, void*>(m_ManagedPtr) : method.Address<void, void*>(m_ManagedPtr);
}