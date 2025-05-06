#pragma once

#include <type_traits>

#include "backend/haxsdk_backend.h"

#define CHECK_NULL(p) if (!p) throw System::NullReferenceException::New()
#define CHECK_RANGE(i, l, r) if (i < l || i > r) throw System::ArgumentOutOfRangeException::New()

using Char = wchar_t;
using Int32 = int;
using Single = float;

namespace System
{
    struct Assembly;
    struct AssemblyName;
    struct FieldInfo;
    struct Object;
    struct Type;
    struct GCHandle;

    enum GCHandleType
    {
        Weak,
        Normal
    };

    struct AppDomain
    {
        explicit AppDomain(backend::Domain* domain) : m_Domain(domain) {}

        static inline AppDomain GetCurrentDomain() { return AppDomain(backend::Domain::GetCurrent()); }
        static inline AppDomain GetRootDomain() { return AppDomain(backend::Domain::GetRoot()); }

        Assembly Load(const char* name, bool doAssert = false);

        backend::Domain* m_Domain;
    };

    struct Object
    {
        explicit Object(void* managedPtr) : m_ManagedPtr(managedPtr) {}

        Type GetType();

        union
        {
            void* m_ManagedPtr;
            backend::String* m_String;
            backend::Exception* m_Exception;
            backend::ReflectionType* m_Type;
            backend::Object* m_Object;
        };
    };

    struct GCHandle
    {
        GCHandle(uint32_t handle) : m_Handle(handle) {}
        ~GCHandle() { Free(); }

        GCHandle(const GCHandle&) = delete;
        GCHandle(GCHandle&& other) noexcept { m_Handle = other.m_Handle; other.m_Handle = 0; }

        GCHandle& operator=(const GCHandle&) = delete;
        GCHandle& operator=(GCHandle&& other) noexcept
        {
            if (&other != this)
            {
                m_Handle = other.m_Handle; 
                other.m_Handle = 0;
            }
            return *this;
        }

        static GCHandle New(const Object& obj)                      { return GCHandle(GetTargetHandle(obj, GCHandleType::Normal)); }
        static GCHandle New(const Object& obj, GCHandleType type)   { return GCHandle(GetTargetHandle(obj, type)); }
        static uint32_t GetTargetHandle(const Object& obj, GCHandleType type);
        Object GetTarget() 
        { 
            if (!m_Handle) 
                return Object(nullptr); 
            return Object(backend::GCHandle::GetTarget(m_Handle)); 
        }
        
    public:
        void Free();

        uint32_t m_Handle;
    };

    struct Exception : Object
    {
        explicit Exception(backend::Exception* ex) : Object(ex) {}

        const char* GetMessage();
    };

    struct NullReferenceException : Exception
    {
        explicit NullReferenceException(backend::Exception* ex) : Exception(ex) {}

        static NullReferenceException New();
    };

    struct ArgumentOutOfRangeException : Exception
    {
        explicit ArgumentOutOfRangeException(backend::Exception* ex) : Exception(ex) {}

        static ArgumentOutOfRangeException New();
    };

    struct FieldInfo
    {
        explicit FieldInfo(backend::Field* field) : m_Field(field) {}
        explicit FieldInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name);

        template <typename T>
        inline T& GetValue(Object __this)
        {
            if (!m_Field)
                throw NullReferenceException::New();
            return *(T*)backend::Field::GetAddress(m_Field, __this.m_Object);
        }

        template <typename T>
        inline T& GetValue(backend::Object* __this)
        {
            if (!m_Field)
                throw NullReferenceException::New();
            return *(T*)backend::Field::GetAddress(m_Field, __this);
        }

        backend::Field* m_Field = nullptr;
    };

    struct MethodInfo
    {
        MethodInfo(backend::Method* method) : m_Method(method) {}
        MethodInfo(const char* assembly, const char* nameSpace, const char* klass, const char* name, const char* signature);

        backend::Method* m_Method = nullptr;
    };

    struct Type : Object
    {
        explicit Type(const char* assembly, const char* nameSpace, const char* name);
        explicit Type(backend::ReflectionType* type) : Object(type) {}

        //Object CreateInstance();
        FieldInfo GetField(const char* name, bool doAssert = false);
        MethodInfo GetMethod(const char* name, const char* signature = nullptr, bool doAssert = false);
    };

    struct Assembly
    {
        //AssemblyName GetName();
        Type GetType(const char* nameSpace, const char* klass, bool doAssert = false);

        backend::Image* m_Image;
    };

    struct AssemblyName
    {
        const char* GetName();
    };

    struct String : Object
    {
        inline const Char&  operator[](int32_t i) const     { CHECK_NULL(m_String); CHECK_RANGE(i, 0, m_String->m_Length - 1); return m_String->operator[](i); }
        inline Char&        operator[](int32_t i)           { CHECK_NULL(m_String); CHECK_RANGE(i, 0, m_String->m_Length - 1); return m_String->operator[](i); }

        inline Char*        begin()                         { CHECK_NULL(m_String); return m_String->begin(); }
        inline const Char*  begin() const                   { CHECK_NULL(m_String); return m_String->begin(); }
        inline Char*        end()                           { CHECK_NULL(m_String); return m_String->end(); }
        inline const Char*  end() const                     { CHECK_NULL(m_String); return m_String->end(); }

        inline Char*        GetRawStringData()              { CHECK_NULL(m_String); return m_String->m_Chars; }
        inline int          GetLength()                     { CHECK_NULL(m_String); return m_String->m_Length; }
    };

    template <typename T>
    struct Array
    {
        inline T&           operator[](int32_t i)           { CHECK_NULL(m_Array); CHECK_RANGE(i, 0, m_Array->m_Length - 1); return m_Array->operator[](i); }
        inline const T&     operator[](int32_t i) const     { CHECK_NULL(m_Array); CHECK_RANGE(i, 0, m_Array->m_Length - 1); return m_Array->operator[](i); }

        inline T*           begin()                         { CHECK_NULL(m_Array); return m_Array->begin(); }
        inline const T*     begin() const                   { CHECK_NULL(m_Array); return m_Array->begin(); }
        inline T*           end()                           { CHECK_NULL(m_Array); return m_Array->begin() + m_Array->m_Length; }
        inline const T*     end() const                     { CHECK_NULL(m_Array); return m_Array->begin() + m_Array->m_Length; }

        backend::Array<T>*  m_Array;
    };

    template <typename T>
    struct List : Object
    {
        inline backend::List<T>*   ToList() { return backend::List<T>*(m_ManagedPtr); };

        inline T&           operator[](int32_t i)           { CHECK_NULL(ToList()); CHECK_RANGE(i, 0, ToList()->m_Length - 1); return ToList()->operator[](i); }
        inline const T&     operator[](int32_t i) const     { CHECK_NULL(ToList()); CHECK_RANGE(i, 0, ToList()->m_Length - 1); return ToList()->operator[](i); }

        inline T*           begin()                         { CHECK_NULL(ToList()); return ToList()->begin(); }
        inline const T*     begin() const                   { CHECK_NULL(ToList()); return ToList()->begin(); }
        inline T*           end()                           { CHECK_NULL(ToList()); return ToList()->begin() + ToList()->m_Length; }
        inline const T*     end() const                     { CHECK_NULL(ToList()); return ToList()->begin() + ToList()->m_Length; }
    };
};

inline const System::Object null = System::Object(nullptr);

template <typename T>
System::Type typeof = System::Type();

template <>
inline System::Type typeof<int> = System::Type("mscorlib", "System", "Int32");

template <>
inline System::Type typeof<float> = System::Type("mscorlib", "System", "Single");

template <>
inline System::Type typeof<System::String> = System::Type("mscorlib", "System", "String");

namespace HaxSdk
{
    void InitSystem();
    System::Assembly GetMscorlib();
}