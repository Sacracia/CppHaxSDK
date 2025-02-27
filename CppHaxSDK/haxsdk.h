#pragma once

// [SECTION] Headers
// [SECTION] Forward declarations and basic types
// [SECTION] Dear ImGui end-user API functions

#include <cstdint>

#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR, _TEXT) if (!_EXPR) \
                                    _wassert(_CRT_WIDE(_TEXT), L"?", (unsigned)(__LINE__))
#endif

#ifndef OUT
#define OUT
#endif

//-----------------------------------------------------------------------------
// [SECTION] Forward declarations and basic types
//-----------------------------------------------------------------------------
// 
// .NET Framework basic system types
typedef bool Boolean;
typedef int8_t Byte;
typedef uint8_t SByte;
typedef float Single;
typedef int16_t Int16;
typedef uint16_t UInt16;
typedef int32_t Int32;
typedef uint32_t UInt32;
typedef int64_t Int64;
typedef uint64_t UInt64;
typedef char16_t Char;

// HaxSdk custom classes and enums
enum HaxBackend;
struct HaxGlobals;

// Mono / IL2CPP basic classes
struct Assembly;
struct Class;
struct Domain;
struct Field;
struct Image;
struct Method;
struct Thread;
struct Type;

// C# basic system classes
namespace System {
    template <class T> struct Array;
    template <class TKey, class TValue> struct Dictionary;
    struct Enum;
    template <class T> struct List;
    struct Object;
    struct String;
    struct Type;
}

//-----------------------------------------------------------------------------
// [SECTION] HaxSdk end-user API functions
//-----------------------------------------------------------------------------

enum HaxBackend {
    HaxBackend_None = 0,
    HaxBackend_Mono = 1 << 0,
    HaxBackend_IL2CPP = 1 << 1
};

struct HaxGlobals {
    HaxBackend backend;
    void* backendHandle;
    bool visible;
    int hotkey = 0xC0;
    void* cheatModule;
};

namespace HaxSdk {
    HaxGlobals& GetGlobals();
    void InitializeCore();
    void UnityAttachThread();
}

struct Assembly {
    static Assembly* Find(const char* name);
    static bool Exists(const char* name, OUT Assembly*& pRes);
public:
    Image* GetImage();
};

//-----------------------------------------------------------------------------
// [CLASS]  TYPE
// [MONO]   https://github.com/mono/mono/blob/0f53e9e151d92944cacab3e24ac359410c606df6/mono/metadata/metadata-internals.h#L24
// [IL2CPP] https://github.com/dreamanlan/il2cpp_ref/blob/09316fe508773b8ced098dae6147b44ee1f6516c/libil2cpp/il2cpp-runtime-metadata.h#L69
//-----------------------------------------------------------------------------

// Represents internal mono type. See backend definitions from above.
// The content of the class differs from backend to backend, but for us it is useless as we use Type only for getting System.Type
struct Type {
    System::Type* GetSystemType();
private:
    struct Il2CppType {
        void*                   data;
        unsigned int            attrs : 16;
        int                     type : 8;
        unsigned int            num_mods : 5;
        unsigned int            byref : 1;
        unsigned int            pinned : 1;
        unsigned int            valuetype : 1;
    };
    struct MonoType {
    };
private:
    union {
        Il2CppType il2cpp;
        MonoType mono;
    };
};

struct Class {
    static bool Exists(const char* assembly, const char* nameSpace, const char* name, OUT Class*& pClass);
    static Class* Find(const char* assembly, const char* nameSpace, const char* name);
public:
    Method* FindMethod(const char* name, const char* signature = nullptr);
    Field* FindField(const char* name);
    void* FindStaticField(const char* name);
    void* GetStaticFieldsData();
    const char* GetName();
    System::Type* GetSystemType();
};

struct Domain {
    static Domain* Main();
public:
    void AttachThread();
};

struct Field {
    int32_t Offset();
    void GetStaticValue(void* pValue);
    void SetStaticValue(void* pValue);
};

struct Method {
    System::Object* Invoke(void* __this, void** ppArgs);
    void* GetAddress();
};

template <typename T>
struct HaxMethod {
    HaxMethod(Method* pMethod) { ptr = orig = (T)pMethod->GetAddress(); pBase = pMethod; }
public:
    inline System::Object* Invoke(void* __this, void** args) { return pBase->Invoke(__this, args); }
public:
    Method* pBase;
    T ptr;
    T orig;
};

struct Thread {
};

struct System::Object {
    static Object* New(Class* pClass);
public:
    System::Object* Ctor();
    void* Unbox();
public:
    Class** ppClass;
    void* monitor;
};

//-----------------------------------------------------------------------------
// [CLASS] System.Array
// [MONO ] https://github.com/mono/mono/blob/0f53e9e151d92944cacab3e24ac359410c606df6/mono/metadata/object-internals.h#L169 
// struct _MonoArray {
//     MonoObject obj;
//     MonoArrayBounds* bounds;
//     mono_array_size_t max_length;
//     mono_64bitaligned_t vector[MONO_ZERO_LEN_ARRAY];
// };
// 
// [IL2CPP] https://github.com/dreamanlan/il2cpp_ref/blob/09316fe508773b8ced098dae6147b44ee1f6516c/libil2cpp/il2cpp-runtime-metadata.h#L68
// typedef struct Il2CppArraySize
// {
//     Il2CppObject obj;
//     Il2CppArrayBounds* bounds;
//     il2cpp_array_size_t max_length;
//     ALIGN_TYPE(8) void* vector[IL2CPP_ZERO_LEN_ARRAY];
// } Il2CppArraySize;
//-----------------------------------------------------------------------------

// Represents .NET Framework System.Array
// May be unsafe when T is not a pointer, but generally it is.
template <class T> 
struct System::Array : System::Object {
    void* pBounds;
    size_t length;
    T vector[0]; // We dont care about instantiating Array, so 0-length is ok.
};

// Represents .NET Framework System.Collections.Generic.Dictionary
// Works only for .NET Framework 4
//template <class TKey, class TValue>
//struct System::Dictionary : System::Object {
//    struct Entry {
//        int32_t hashCode;
//        int32_t next;
//        TKey key;
//        TValue value;
//    };
//public:
//    System::Array<Entry>* GetEntries() { return entries; }
//    int32_t Count() { return count; }
//private:
//    System::Array<int>* buckets;
//    System::Array<Entry>* entries;
//    int32_t count;
//};

// Represents .NET Framework System.Enum
//struct System::Enum : System::Object {
//    static Array<String*>* GetNames(System::Type* pType);
//    static Array<int32_t>* GetValues(System::Type* pType);
//};

template <class T> 
struct System::List : System::Object {
    Array<T>* pItems;
    Int32 length;
    Int32 version;
    void* pSyncRoot;
};

struct System::String : System::Object {
    static System::String* New(const char* data);
    Int32 length;
    Char chars[1];
};

struct System::Type : System::Object {
    ::Type* pType;
};