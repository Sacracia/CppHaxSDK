#pragma once

#include <cstdint>

#undef GetMessage

enum HaxBackend
{
    HaxBackend_None,
    HaxBackend_Mono,
    HaxBackend_Il2Cpp
};

namespace HaxSdk
{
    void InitBackend();
    void AttachThread();
    bool IsMono();
    bool IsIl2Cpp();
    void* GetBackendHandle();
}

namespace backend
{
    struct Assembly;
    struct Class;
    struct Domain;
    struct Field;
    struct Image;
    struct Method;
    struct Object;
    struct Runtime;
    struct ReflectionType;
    struct Signature;
    struct String;
    struct Thread;
    struct Type;
    struct Reflection;
    struct VTable;

    struct Assembly
    {
        static Image* GetImage(Assembly* assembly);
        static Assembly* Load(const char* name);
    };

    struct Class
    {
        static Class* FromName(Image* image, const char* nameSpace, const char* name);
        static Field* GetFieldFromName(Class* klass, const char* name);
        static Method* GetMethodFromName(Class* klass, const char* name, const char* signature);
        //static const char* GetName(Class* klass);
        //static const char* GetNamespace(Class* klass);
        static Type* GetType(Class* klass);
    };

    struct Domain
    {
        static Domain* GetCurrent();
        static Domain* GetRoot();
    };

    struct Exception
    {
        static const char* GetMessage(Exception* ex);
        static Exception* GetNullReferenceException();
        static Exception* GetArgumentOutOfRangeException();
    };

    struct Field
    {
        static void* GetAddress(Field* field, Object* __this);
        static Class* GetParent(Field* field);
        static void StaticGetValue(Field* field, void* value);
        static void StaticSetValue(Field* field, void* value);
    };

    struct Image
    {
        static Image* GetCorlib();
        static Assembly* GetAssembly(Image* image);
        const char* GetName(Image* image);
    };

    struct Reflection
    {
        static ReflectionType* TypeGetObject(Type* type);
    };

    struct String
    {
        inline wchar_t& operator[](size_t i) { return m_Chars[i]; }
        inline const wchar_t& operator[](size_t i) const { return m_Chars[i]; }

        inline wchar_t* begin() { return m_Chars; }
        inline const wchar_t* begin() const { return m_Chars; }

        inline wchar_t* end() { return m_Chars + m_Length; }
        inline const wchar_t* end() const { return m_Chars + m_Length; }

    private:
        void* __space[2];
    public:
        int m_Length;
        wchar_t m_Chars[1];
    };

    template <typename T>
    struct Array
    {
        inline T& operator[](size_t i) { return m_Items[i]; }
        inline const T& operator[](size_t i) const { return m_Items[i]; }

        inline T* begin() { return &m_Items[0]; }
        inline const T* begin() const { return &m_Items[0]; }
        inline T* end() { return m_Items + m_Length; }
        inline const T* end() const { return m_Items + m_Length; }

    private:
        void* __space[2];
        void* m_Bounds;
    public:
        size_t m_Length;
        __declspec(align(8)) T m_Items[1];
    };

    template <typename T>
    struct List
    {
        inline T& operator[](size_t i) { return m_Items->operator[](i); }
        inline const T& operator[](size_t i) const { return m_Items->operator[](i); }

        inline T* begin() { return m_Items->begin(); }
        inline const T* begin() const { return m_Items->begin(); }
        inline T* end() { return m_Items->begin() + m_Length; }
        inline const T* end() const { return m_Items->begin() + m_Length; }

        inline int32_t GetLength() { return m_Length; }
        inline int32_t GetCapacity() { return m_Items->m_Length; }

    private:
        void* __space[2];
    public:
        Array<T>* m_Items;
        int32_t m_Length;
        int32_t m_Version;
        void* m_SyncRoot;
    };

    struct Object
    {
        static ReflectionType* GetType(Object* object);
        static Object* Box(Class* klass, void* data);
        static Class* GetClass(Object* object);
        static Object* New(Domain* domain, Class* klass);
        static void* Unbox(Object* object);
    };

    struct GCHandle
    {
        static uint32_t New(Object* obj, bool pinned);
        static uint32_t NewWeakRef(Object* obj, bool trackResurrection);
        static Object* GetTarget(uint32_t handle);
        static void Free(uint32_t handle);
    };

    struct Type
    {
        static char* GetName(Type* type);
    };

    struct ReflectionType
    {
        static Class* GetClass(ReflectionType* type);
    };

    struct Vector3_Boxed
    {
        Vector3_Boxed(float _x, float _y, float _z);

    private:
        void* metadata;
        void* monitor;
        float x, y, z;
    };

    /*struct Method
    {
        static void* Compile(Method* method);
        static const char* GetName(Method* method);
        static void* GetUnmanagedThunk(Method* method);
        static Signature* Signature(Method* method);
    };

    struct Runtime
    {
        static Object* Invoke(Method* method, void* __this, void** args, Exception** ex);
        static void ObjectInit(Object* object);
    };

    struct Signature
    {
        static uint32_t GetParamCount(Signature* methodSig);
        static Type* GetParams(Signature* methodSig, void** iter);
        static Type* GetReturnType(Signature* methodSig);
    };

    struct String
    {
        static String* New(Domain* domain, const char* str);
        static char* ToUTF8(String* str);
    };

    struct Thread
    {
        static Thread* Attach(Domain* domain);
        static Thread* Current();
        static void Detach(Thread* thread);
    };

    struct Reflection
    {
        static ReflectionType* GetTypeObject(Domain* domain, Type* type);
    };

    struct VTable
    {
        static void* GetStaticFieldData(VTable* vtable);
    };*/
}