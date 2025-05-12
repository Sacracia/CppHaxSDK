#pragma once

#include <type_traits>

#include "../backend/haxsdk_backend.h"
#include "../haxsdk_tools.h"
#include "../haxsdk_assertion.h"

#define CHECK_NULL() if (Null()) HaxSdk::ThrowNullRef();
#define CHECK_RANGE(i, l, r) if (i < l || i > r) HaxSdk::ThrowOutOfRange();

using Char = wchar_t;
using Int32 = int;
using Single = float;

namespace HaxSdk
{
    void InitSystem();
    void ThrowNullRef();
    void ThrowOutOfRange();
}

namespace System
{   
    struct Type;
    struct MethodInfo;

    struct Object
    {
        explicit                Object() = default;
        explicit constexpr      Object(nullptr_t ptr) : m_Pointer(ptr) {}
        explicit                Object(unsafe::Object* ptr) : m_Pointer(ptr) {}
        explicit                Object(unsafe::Exception* ptr) : m_Pointer(ptr) {}
        explicit                Object(unsafe::String* ptr) : m_Pointer(ptr) {}

        inline                  operator bool() const       { return m_Pointer != nullptr; }

        inline bool             Null() const                { return m_Pointer == nullptr; }
        inline unsafe::Object*  GetPointer() const          { return m_Pointer; }

        Type                    GetType();

    protected:
        unsafe::Object*         m_Pointer;
    };

    struct String : Object
    {
        explicit                String(unsafe::Object* ptr) : Object(ptr) {}
        explicit                String(unsafe::String* ptr) : Object(ptr) {}

        static inline String    New(const char* str)    { return String(unsafe::String::New(str)); }

        inline unsafe::String*  GetPointer() const      { return (unsafe::String*)m_Pointer; }

        Char&                   operator[](int i);
        const Char&             operator[](int i) const;

                                operator bool() const   { return m_Pointer != nullptr; }

        Char*                   begin();
        const Char*             begin() const;
        Char*                   end();
        const Char*             end() const;

        Int32                   GetLength() const;
        Char*                   GetRawStringData() const;
        const char*             UTF8() const;
    };

    template <typename T>
    struct Array : Object
    {
        explicit                Array(unsafe::Object* ptr) : Object(ptr) {}
        explicit                Array(unsafe::Array<T>* ptr) : Object(ptr) {}

        inline unsafe::Array<T>*   GetPointer() const       { return (unsafe::Array<T>*)m_Pointer; }

        inline T&               operator[](size_t i)        { CHECK_NULL(); CHECK_RANGE(i + 1, 1, GetPointer()->GetLength()); return GetPointer()->operator[](i); }
        inline const T&         operator[](size_t i) const  { CHECK_NULL(); CHECK_RANGE(i + 1, 1, GetPointer()->GetLength()); return GetPointer()->operator[](i); }
    
        inline                  operator bool() const       { return m_Pointer != nullptr; }

        inline T*               begin()                     { CHECK_NULL(); return GetPointer()->begin(); }
        inline const T*         begin() const               { CHECK_NULL(); return GetPointer()->begin(); }
        inline T*               end()                       { CHECK_NULL(); return GetPointer()->end(); }
        inline const T*         end() const                 { CHECK_NULL(); return GetPointer()->end(); } 

        inline size_t           GetLength() const           { CHECK_NULL(); return GetPointer()->GetLength(); }
    };

    template <typename T>
    struct List : Object
    {
        explicit            List(unsafe::Object* ptr) : Object(ptr) {}
        explicit            List(unsafe::List<T>* ptr) : Object(ptr) {}

        inline unsafe::List<T>* GetPointer() const      { return (unsafe::List<T>*)m_Pointer; }

        inline T&           operator[](size_t i)        { CHECK_NULL(); CHECK_RANGE(i, 0, GetPointer()->GetLength() - 1); return GetPointer()->operator[](i); }
        inline const T&     operator[](size_t i) const  { CHECK_NULL(); CHECK_RANGE(i, 0, GetPointer()->GetLength() - 1); return GetPointer()->operator[](i); }

        inline              operator bool() const       { return m_Pointer != nullptr; }

        inline T*           begin()                     { CHECK_NULL(); return GetPointer()->begin(); }
        inline const T*     begin() const               { CHECK_NULL(); return GetPointer()->begin(); }
        inline T*           end()                       { CHECK_NULL(); return GetPointer()->end(); }
        inline const T*     end() const                 { CHECK_NULL(); return GetPointer()->end(); }

        inline int          GetLength() const           { CHECK_NULL(); return GetPointer()->GetLength(); }
        inline int          GetCapacity() const         { CHECK_NULL(); return GetPointer()->GetCapacity(); }
    
        bool                Contains(T val);
    };

    struct FieldInfo
    {
        explicit            FieldInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name);
        explicit            FieldInfo(unsafe::Field* ptr) : m_Pointer(ptr) {}

        inline bool         IsLiteral()                 { CHECK_NULL(); return m_Pointer->IsLiteral(); }
        inline bool         IsStatic()                  { CHECK_NULL(); return m_Pointer->IsStatic(); }
        inline Int32        GetOffset()                 { CHECK_NULL();  return m_Pointer->GetOffset(); }
        inline bool         Null()                      { return m_Pointer == nullptr; }

        inline Int32        GetEnumValue()              { CHECK_NULL(); Int32 v; m_Pointer->GetStaticValue(&v); return v; }
        inline unsafe::Field* GetPointer() const        { return m_Pointer; }

        void*               GetValuePtr(const Object& __this);

    private:
        unsafe::Field* m_Pointer;
    };

    struct Type : Object
    {
        explicit Type(unsafe::Object* ptr) : Object(ptr) {}
        explicit Type(const char* assembly, const char* nameSpace, const char* name);
        explicit Type(unsafe::ReflectionType* ptr) : Object(ptr) {}

        inline operator bool() const { return !Null(); }

        inline FieldInfo GetField(const char* name, bool doAssert = true) { CHECK_NULL(); return FieldInfo(GetPointer()->GetType()->GetClass()->GetField(name, doAssert)); }
        MethodInfo GetMethod(const char* name, const char* sig = nullptr, bool doAssert = true);

        inline unsafe::ReflectionType* GetPointer() const { return (unsafe::ReflectionType*)m_Pointer; }
    };

    struct Assembly
    {
        explicit Assembly(unsafe::Image* ptr) : m_Pointer(ptr) {}

        Type GetType(const char* nameSpace, const char* name, bool doAssert = true);
        inline bool         Null() { return m_Pointer == nullptr; }

    private:
        unsafe::Image* m_Pointer;
    };

    struct AppDomain
    {
        explicit AppDomain(unsafe::Domain* ptr) : m_Pointer(ptr) {}

        static inline AppDomain GetCurrent() { return AppDomain(unsafe::Domain::GetRoot()); }
        
        inline Assembly Load(const char* name, bool doAssert = true) { CHECK_NULL(); return Assembly(m_Pointer->GetImage(name, doAssert)); }
        inline bool Null() { return m_Pointer == nullptr; }

    private:
        unsafe::Domain* m_Pointer;
    };

    struct Exception : Object
    {
        explicit                Exception(unsafe::Exception* ptr) : Object(ptr) {}
        explicit                Exception(const Il2CppExceptionWrapper& ex) : Object((unsafe::Exception*)ex.ex) {}

        inline operator bool() const { return !Null(); }

        inline String           GetMessage()            { CHECK_NULL(); return String(GetPointer()->GetMessage()); }
        inline String           GetStackTrace()         { CHECK_NULL(); return String(GetPointer()->GetStackTrace()); }

        inline unsafe::Exception* GetPointer() const          { return (unsafe::Exception*)m_Pointer; }
    };

    struct NullReferenceException : Exception
    {
        explicit NullReferenceException(unsafe::Exception* ptr) : Exception(ptr) {}

        inline operator bool() const { return !Null(); }

        static NullReferenceException New() { return NullReferenceException(unsafe::Exception::GetNullReference()); }
    };

    struct ArgumentOutOfRangeException : Exception
    {
        explicit ArgumentOutOfRangeException(unsafe::Exception* ptr) : Exception(ptr) {}

        inline operator bool() const { return !Null(); }

        static ArgumentOutOfRangeException New() { return ArgumentOutOfRangeException(unsafe::Exception::GetArgumentOutOfRange()); }
    };

    struct TargetException : Exception
    {
        explicit TargetException(unsafe::Exception* ptr) : Exception(ptr) {}

        inline operator bool() const { return !Null(); }

        static TargetException New(const String& message) { return TargetException(unsafe::Exception::GetTargetException(message.GetPointer())); }
    };

    struct Int32_Boxed : unsafe::Object
    {
        Int32_Boxed(int v);

        int m_Value;
    };

    struct MethodInfo
    {
        explicit MethodInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* sig);

        explicit MethodInfo(unsafe::Method* base) : m_Base(base)
        {
            m_Pointer = m_Orig = base->GetPointer();
            m_Thunk = base->GetThunk();
        }

        template <typename Ret, typename... Args>
        Ret Address(const Args&... args)
        {
            CHECK_NULL();
            try
            {
                if constexpr (std::is_void_v<Ret>)
                {
                    ((void(*)(Args...))m_Pointer)(args...);
                    return;
                }
                else if constexpr (std::is_base_of_v<Object, Ret>)
                {
                    unsafe::Object* res = ((unsafe::Object*(*)(Args...))m_Pointer)(args...);
                    return Ret(res);
                }
                else
                {
                    return ((Ret(*)(Args...))m_Pointer)(args...);
                }
            }
            catch (const Il2CppExceptionWrapper& ex)
            {
                throw System::Exception(ex);
            }
        }

        template <typename Ret, typename... Args>
        Ret Thunk(const Args&... args)
        {
            HAX_ASSERT_E(m_Thunk, "Thunk is not available for your game");
            CHECK_NULL();
            Exception ex(nullptr);
            if constexpr (std::is_void_v<Ret>)
            {
                ((void(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                    throw ex;
                return;
            }
            else if constexpr (std::is_base_of_v<Object, Ret>)
            {
                void* res = ((void*(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                    throw ex;
                return Ret((unsafe::Object*)res);
            }
            else
            {
                Ret res = ((Ret(*)(Args..., Exception*))m_Thunk)(args..., &ex);
                if (ex)
                    throw ex;
                return res;
            }
        }

        template <typename Ret, typename... Args>
        Ret Invoke(const Args&... args)
        {
            CHECK_NULL();
            static_assert(sizeof... (args) > 0);
            
            unsafe::Exception* ex = nullptr;
            void* packedArgs[] = { Pack(args)... };
            int nArgs = sizeof(packedArgs) / sizeof(void*);
            unsafe::Object* res = m_Base->Invoke(packedArgs[0], nArgs == 1 ? nullptr : packedArgs + 1, &ex);
            
            if (ex)
                throw System::Exception(ex);

            if constexpr (std::is_void_v<Ret>) return;
            else if constexpr (std::is_base_of_v<System::Object, Ret>) return Ret(res);
            else return static_cast<Ret>(res);
        }
            
        template <typename Ret, typename... Args>
        Ret InvokeStatic(const Args&... args)
        {
            CHECK_NULL();
            if constexpr (sizeof...(args) == 0)
            {
                unsafe::Exception* ex = nullptr;
                unsafe::Object* res = m_Base->Invoke(nullptr, nullptr, &ex);

                if (ex)
                    throw System::Exception(ex);

                if constexpr (std::is_void_v<Ret>)
                    return;
                else if constexpr (std::is_base_of_v<System::Object, Ret>)
                    return Ret(res);
                else
                    return static_cast<Ret>(res);
            }
            else
            {
                unsafe::Exception* ex = nullptr;
                void* packedArgs[] = { Pack(args)... };
                unsafe::Object* res = m_Base->Invoke(nullptr, packedArgs, &ex);

                if (ex)
                    throw System::Exception(ex);

                if constexpr (std::is_void_v<Ret>)
                    return;
                else if constexpr (std::is_base_of_v<System::Object, Ret>)
                    return Ret(res);
                else
                    return static_cast<Ret>(res);
            }
        }

        inline bool         Null() { return m_Base == nullptr; }

    private:
        template <typename T>
        void* Pack(const T& val)
        {
            if constexpr (std::is_pointer_v<T>) return val;
            if constexpr (std::is_base_of_v<System::Object, T>) return val.GetPointer();
            else return (void*)&val;
        }

    public:
        unsafe::Method* m_Base;
        void*           m_Pointer;
        void*           m_Thunk;
        void*           m_Orig;
    };

    template <typename T>
    bool List<T>::Contains(T val)
    {
        CHECK_NULL();

        static MethodInfo method = this->GetType().GetMethod("Contains");
        if (HaxSdk::IsMono())
        {
            return method.m_Thunk
                ? method.Thunk<bool, List<T>, T>(*this, val)
                : method.Invoke<bool, List<T>, T>(*this, val);
        }
        return method.Address<bool, List<T>, T>(*this, val);
    }
}

inline constexpr System::Object null(nullptr);

template <typename T>
System::Type typeof = System::Type();

template <>
inline System::Type typeof<int> = System::Type("mscorlib", "System", "Int32");

template <>
inline System::Type typeof<float> = System::Type("mscorlib", "System", "Single");

template <>
inline System::Type typeof<System::String> = System::Type("mscorlib", "System", "String");