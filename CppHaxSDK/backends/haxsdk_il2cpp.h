#pragma once

#include <stdint.h>

struct  Il2CppObject;
struct  Il2CppMethod;
struct  Il2CppClass;
struct  Il2CppType;
struct  Il2CppParameter;
struct  Il2CppClassField;
struct  Il2CppImage;
struct  Il2CppThread;
typedef Il2CppClass  BackendClass;
typedef Il2CppObject BackendObject;
typedef Il2CppMethod BackendMethod;
typedef Il2CppObject SystemType;

namespace HaxSdk {
    void InitializeBackend();
    void AttachToThread();
}

struct Il2CppObject {
    Il2CppClass*            klass() { return m_class; }
public:
    static Il2CppObject*    alloc(Il2CppClass* klass);
    void*                   unbox();
private:
    Il2CppClass*            m_class;
    void*                   m_monitor;
};

struct Il2CppMethod final {
    void*                   address() { return m_address; }
    Il2CppClass*            klass();
    const char*             name();
    void                    signature(char* buff);
public:
    Il2CppObject*           invoke(void* obj, void** args);
private:
    void*                   m_address;
};

struct Il2CppType {
    Il2CppObject* system_type();
private:
    void*                   data;
    unsigned int            attrs     : 16;
    int                     type      : 8;
    unsigned int            num_mods  : 5;
    unsigned int            byref     : 1;
    unsigned int            pinned    : 1;
    unsigned int            valuetype : 1;
};

struct Il2CppClass final {
    const char*             name()      { return m_name; }
    const char*             namespaze() { return m_namespace; }
    Il2CppType*             type();
public:
    static Il2CppClass*     find(const char* assembly, const char* name_space, const char* name);
    Il2CppMethod*           find_method(const char* name, const char* params);
    void*                   find_static_field(const char* name);
    Il2CppClassField*       find_field(const char* name);
private:
    Il2CppImage*            m_image;
    void*                   m_gc_desc;
    const char*             m_name;
    const char*             m_namespace;
    Il2CppType              m_byval_arg;
    Il2CppType              m_this_arg;
    void*                   __space[15];
    void*                   m_static_fields;
    //...
};

struct Il2CppClassField {
    int                     offset() { return m_offset; }
    const char*             name()   { return m_name; }
    Il2CppType*             type()   { return m_type; }
private:
    char*                   m_name;
    Il2CppType*             m_type;
    Il2CppClass*            m_parent;
    int32_t                 m_offset;
    uint32_t                m_token;
};

struct Il2CppAssembly {
    friend struct           Il2CppDomain;
    Il2CppClass*            find_class(const char* name_space, const char* name);
private:
    Il2CppImage*            m_image;
    uint32_t                m_token;
    int32_t                 m_referencedAssemblyStart;
    int32_t                 m_referencedAssemblyCount;
    const char*             m_name;
    //...
};

struct Il2CppDomain {
    static Il2CppDomain*    current();
    Il2CppAssembly*         find_assembly(const char* assembly);
    void                    attach_thread();
};

struct Il2CppArrayBounds {
    size_t                  m_length;
    int32_t                 m_lower_bound;
};

template <class T>
struct Il2CppArray {
    Il2CppObject            m_object;
    Il2CppArrayBounds*      m_bounds;
    size_t                  m_max_length;
    T                       m_vector[32];
};

struct Il2CppString {
    wchar_t*                data()   { return m_chars; }
    int32_t                 length() { return m_length; }
public:
    static Il2CppString*    alloc(const char* raw);
private:
    Il2CppObject            m_object;
    int32_t                 m_length;
    wchar_t                 m_chars[32];
};