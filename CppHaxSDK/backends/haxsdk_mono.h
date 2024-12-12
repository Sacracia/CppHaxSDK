#pragma once

#include <stdint.h>

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS OF BASE STRUCTURES
//-------------------------------------------------------------------------
struct  MonoObject;
struct  MonoThreadsSync;
struct  MonoArrayBounds;
struct  MonoType;
struct  MonoString;
struct  MonoClassField;
struct  MonoMethod;
struct  MonoAssembly;
struct  MonoClass;
struct  MonoReflectionType;
struct  MonoJitInfo;
struct  MonoImage;
struct  MonoThread;
struct  MonoVTable;
struct  MonoMethodSignature;
typedef MonoClass  BackendClass;
typedef MonoMethod BackendMethod;
typedef MonoObject BackendObject;
typedef MonoReflectionType SystemType;

namespace HaxSdk {
    void InitializeBackendData();
    void AttachToThread();
}

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS OF BASE STRUCTURES
//-------------------------------------------------------------------------

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
    void*                   address();
    MonoClass*              klass() { return m_class; }
    const char*             name();
    void                    signature(char* buff);
    MonoObject*             invoke(void* obj, void** args);
private:
    uint16_t                m_flags;
    uint16_t                m_iflags;
    uint32_t                m_token;
    MonoClass*              m_class;
};

struct MonoAssembly {
    MonoClass*              find_class(const char* name_space, const char* name);
};

struct MonoType {
    MonoReflectionType*     system_type();
};

struct MonoClass {
    static MonoClass*       find(const char* assembly, const char* name_space, const char* name);
    const char*             name()      { return m_name;}
    const char*             namespaze() { return m_namespace;}
    MonoType*               type();
    MonoMethod*             find_method(const char* name, const char* params);
    void*                   find_static_field(const char* name);
    MonoClassField*         find_field(const char* name);
private:
    char                    __space[40];
    MonoImage*              m_image;
    const char*             m_name;
    const char*             m_namespace;
};

struct MonoDomain {
    static MonoDomain*      root();
    static MonoDomain*      current();
    MonoAssembly*           assembly(const char* assembly);
    void                    attach_thread();
};