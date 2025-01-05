#pragma once

#include <stdint.h>
#include <string>

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS OF BASE STRUCTURES
//-------------------------------------------------------------------------
namespace System {
    struct Object;
    struct Type;
    struct String;
    template <class T> 
    struct Array;
    template <class T> 
    struct List;
    template <class TKey, class TValue> 
    struct Dictionary;
}
struct Field;
struct Method;
struct Assembly;
struct Class;
struct Type;
struct Image;
struct Thread;

namespace HaxSdk {
    void InitializeBackendData();
}

struct System::Object {
    static System::Object*  New(Class* pClass);
    static System::Object*  NewI(Class* pClass);
    void*                   Unbox();
    Class*                  Klass() { return m_class; }
private:
    Class*                  m_class;
    void*                   m_monitor;
};

struct System::String : System::Object {
    static System::String*  New(const char* data);
    static System::String*  Concat(System::String* s1, System::String* s2);
    wchar_t*                Data()   { return m_chars; }
    int32_t                 Length() { return m_length; }
    std::string             UTF8();
private:
    int32_t                 m_length;
    wchar_t                 m_chars[32];
};

template <class T>
struct System::Array : System::Object {
    uintptr_t               Length() { return m_length; }
    T*                      Data() { return m_vector; }
private:
    void*                   m_bounds;
    uintptr_t               m_length;
    T                       m_vector[32]; //!!!!!
};

template <class T>
struct System::List : System::Object {
    int32_t                 Lenght() { return m_size; }
    T*                      Data() { return m_items->Data(); }
private:
    System::Array<T>*       m_items;
    int32_t                 m_size;
    int32_t                 m_version;
    void*                   m_syncRoot;
};

template <class TKey, class TValue>
struct System::Dictionary : System::Object {
    bool                    ContainsKey(TKey key);
    bool                    ContainsKey(TKey* key);
};

struct Domain {
    static Domain*          Current();
    static Assembly*        FindAssembly(const char* assembly);
    static Assembly*        TryFindAssembly(const char* assembly);
    void                    AttachThread();
};

struct Assembly {
    const char*             Name() { return m_name; }
    Class*                  FindClass(const char* name_space, const char* name);
    Class*                  TryFindClass(const char* name_space, const char* name);
private:
    Image*                  m_image;
    uint32_t                m_token;
    int32_t                 m_referencedAssemblyStart;
    int32_t                 m_referencedAssemblyCount;
    const char*             m_name;
};

struct Type {
    System::Type*           SystemType();
private:
    void*                   m_data;
    unsigned int            m_attrs : 16;
    int                     m_type : 8;
    unsigned int            m_num_mods : 5;
    unsigned int            m_byref : 1;
    unsigned int            m_pinned : 1;
    unsigned int            m_valuetype : 1;
};

struct Class {
    static Class*           Find(const char* assembly, const char* name_space, const char* name);
    static Class*           TryFind(const char* assembly, const char* name_space, const char* name);
    const char*             Name() { return m_name; }
    const char*             Namespace() { return m_namespace; }
    System::Type*           SystemType();
    Method                  FindMethod(const char* name, const char* params);
    Method                  FindMethod(const char* name);
    void*                   FindStaticField(const char* name);
    Field*                  FindField(const char* name);
private:
    Image*                  m_image;
    void*                   m_gcDesc;
    const char*             m_name;
    const char*             m_namespace;
    Type                    m_byvalArg;
    Type                    m_thisArg;
    void*                   __space[15];
    void*                   m_staticFields;
};

struct Method {
    friend struct Class;
private:
    struct Il2CppMethod {
        void* m_ptr;
    };
public:
    Method() = default;
    Method(Il2CppMethod* base) : m_base(base), m_ptr(base->m_ptr) {}
public:
    void*&                  Ptr()   { return m_ptr; }
    Class*                  Klass();
    const char*             Name();
    System::Object*         Invoke(void* obj, void** args);
private:
    void                    Signature(char* buff);
private:
    Il2CppMethod*           m_base;
    void*                   m_ptr;
};

struct Field {
    int32_t                 Offset() { return m_offset; }
    const char*             Name()   { return m_name; }
private:
    const char*             m_name;
    Type*                   m_type;
    Class*                  m_parent;
    int32_t                 m_offset;
    uint32_t                m_token;
};


template<class TKey, class TValue>
inline bool System::Dictionary<TKey, TValue>::ContainsKey(TKey key) {
    static Method pFunc = this->Klass()->FindMethod("ContainsKey");
    void* args[1] = { &key };
    return *(bool*)pFunc.Invoke(this, args)->Unbox();
}

template<class TKey, class TValue>
inline bool System::Dictionary<TKey,TValue>::ContainsKey(TKey* key) {
    static Method pFunc = this->Klass()->FindMethod("ContainsKey");
    void* args[1] = { key };
    return *(bool*)pFunc.Invoke(this, args)->Unbox();
}