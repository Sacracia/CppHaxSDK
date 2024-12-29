#pragma once

#include <stdint.h>
#include <iostream>

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS OF BASE STRUCTURES
//-------------------------------------------------------------------------
namespace System {
    struct Object;
    struct Type;
    template <class T> struct Array;
    struct String;
    template <class T> struct List;
    template <class TKey, class TValue> struct Dictionary;
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
    void AttachToThread();
}

struct System::Object {
    static System::Object*  New(Class* pClass);
    static System::Object*  NewI(Class* pClass);
    void*                   Unbox();
    Class*                  Klass() { return *m_class; }
private:
    Class**                 m_class;
    void*                   m_synchronisation;
};

template <class T>
struct System::Array {
    uintptr_t               Length() { return m_length; }
    T*                      Data() { return m_vector; }
private:
    System::Object          m_object;
    void*                   m_bounds;
    uintptr_t               m_length;
    T                       m_vector[32]; //!!!!!
};

template <class T>
struct System::List : System::Object {
    int32_t                 Lenght() { return m_length; }
    T*                      Data() { return m_items->Data(); }
private:
    System::Array<T>*       m_items;
    int32_t                 m_length;
    int32_t                 m_version;
    void*                   m_syncRoot;
};

struct System::String : System::Object {
    static System::String*  New(const char* data);
    static System::String*  Concat(System::String* s1, System::String* s2);
    wchar_t*                Data()   { return m_chars; }
    int32_t                 Length() { return m_length; }
    char*                   UTF8();
private:
    int32_t                 m_length;
    wchar_t                 m_chars[32];
};

template <class TKey, class TValue>
struct System::Dictionary : System::Object {
    bool                    ContainsKey(TKey key);
    bool                    ContainsKey(TKey* key);
};

struct Field {
    int32_t                 Offset() { return m_offset; }
    const char*             Name()   { return m_name; }
    //MonoType*             type()   { return m_type; } 
private:
    Type*                   m_type;
    const char*             m_name;
    Class*                  m_parent;
    int32_t                 m_offset;
};

struct Method {
    friend struct Class;
private:
    struct MonoMethod {
        uint16_t            m_flags;
        uint16_t            m_iflags;
        uint32_t            m_token;
        Class*              m_class;
    };
public:
    Method() = default;
    Method(MonoMethod* base, void* ptr) : m_base(base), m_ptr(ptr) {}
public:
    void*&                  Ptr() { return m_ptr; }
    Class*                  Klass() { return m_base->m_class; }
    const char*             Name();
    System::Object*         Invoke(void* obj, void** args);
private:
    void                    Signature(char* buff);
private:
    MonoMethod*             m_base;
    void*                   m_ptr;
};

struct Assembly {
    Class*                  FindClass(const char* name_space, const char* name);
};

struct Type {
    System::Type*           SystemType();
};

struct Class {
    static Class*           Find(const char* assembly, const char* name_space, const char* name);
    const char*             Name();
    const char*             Namespace();
    System::Type*           Type();
    Method                  FindMethod(const char* name, const char* params);
    Method                  FindMethod(const char* name);
    void*                   FindStaticField(const char* name);
    Field*                  FindField(const char* name);
};

struct Domain {
    static Domain*          Root();
    Assembly*               FindAssembly(const char* assembly);
    bool                    HasAssembly(const char* assembly);
    void                    AttachThread();
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