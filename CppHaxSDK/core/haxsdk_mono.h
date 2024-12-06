#pragma once

#include <stdint.h>

struct  MonoObject;
struct  MonoThreadsSync;
struct  MonoArrayBounds;
struct  MonoType;
//struct MonoArray;
struct  MonoString;
struct  MonoClassField;
struct  MonoMethod;
struct  MonoAssembly;
struct  MonoClass;
struct  MonoReflectionType;
struct  MonoJitInfo;
struct  MonoImage;
struct  MonoThread;

typedef MonoClass  BackendClass;
typedef MonoMethod BackendMethod;
typedef MonoObject BackendObject;

namespace HaxSdk {
    void AttachToThread();
    void InitializeMono();
}

struct MonoObject {
    static MonoObject*      alloc(MonoClass* klass);
    void*                   unbox();
    MonoClass*              klass() { return *m_class; }
private:
    MonoClass**             m_class;
    MonoThreadsSync*        m_synchronisation;
};

struct MonoArrayBounds {
    uint64_t                m_length;
    int64_t                 m_lower_bound;
};

template <class T>
struct MonoArray {
    MonoObject              m_object;
    MonoArrayBounds*        m_bounds;
    uint64_t                m_max_length;
    T                       m_vector[32];
};

struct MonoString {
    static MonoString*      alloc(const char* raw);
    wchar_t*                data()   { return m_chars; }
    int32_t                 length() { return m_length; }
private:
    MonoObject              m_object;
    int32_t                 m_length;
    wchar_t                 m_chars[32];
};

struct MonoClassField {
    int                     offset() { return m_offset; }
    const char*             name()   { return m_name; }
    MonoType*               type()   { return m_type; }
private:
    MonoType*               m_type;
    const char*             m_name;
    MonoClass*              m_parent;
    int                     m_offset;
};

struct MonoMethod final {
    void*                   pointer();
    MonoClass*              klass() { return m_klass; }
    const char*             full_name();
private:
    uint16_t                m_flags;
    uint16_t                m_iflags;
    uint32_t                m_token;
    MonoClass*              m_klass;
};

struct MonoAssembly {
    MonoClass*              klass(const char* name_space, const char* name);
};

struct MonoClass {
    static MonoClass*       find(const char* assembly, const char* name_space, const char* name);
    MonoObject*             ctor();
    MonoMethod*             method(const char* signature);
    MonoMethod*             method(const char* name_space, const char* klass, const char* name, const char* params);
    void*                   static_field(const char* name);
    MonoClassField*         field(const char* name);
    MonoReflectionType*     system_type();
};

struct MonoDomain {
    static MonoDomain*      root();
    static MonoDomain*      current();
    MonoVTable*             vtable(MonoClass* pClass);
    MonoAssembly*           assembly(const char* assembly);
    void                    attach_thread();
};