#pragma once

// [SECTION] Headers
// [SECTION] Forward declarations and basic types
// [SECTION] Dear ImGui end-user API functions

#include <cstdint>
#include <string_view>

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
struct HaxLogger;

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
    bool visible = true;
    int hotkey = 0xC0;
    void* cheatModule;
};

namespace HaxSdk {
    HaxGlobals& GetGlobals();
    void InitLogger(bool useConsole);
    void Log(std::string_view text);
    void InitializeCore();
    void UnityAttachThread();
}

struct Assembly {
    Assembly() = delete;
    Assembly(const Assembly&) = delete;

    static Assembly* Find(const char* name);
    static bool Exists(const char* name, OUT Assembly*& pRes);

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
    struct Il2CppType {
        void* data;
        unsigned int            attrs : 16;
        int                     type : 8;
        unsigned int            num_mods : 5;
        unsigned int            byref : 1;
        unsigned int            pinned : 1;
        unsigned int            valuetype : 1;
    };

    struct MonoType {
    };

    Type() = delete;
    Type(const Type&) = delete;

    System::Type* GetSystemType();
private:
    union {
        Il2CppType il2cpp;
        MonoType mono;
    };
};

struct Class {
    Class() = delete;
    Class(const Class&) = delete;

    static bool Exists(const char* assembly, const char* nameSpace, const char* name, OUT Class*& pClass);
    static Class* Find(const char* assembly, const char* nameSpace, const char* name);

    Method* FindMethod(const char* name, const char* signature = nullptr);
    Field* FindField(const char* name);
    void* FindStaticField(const char* name);
    void* GetStaticFieldsData();
    const char* GetName();
    System::Type* GetSystemType();
};

struct Domain {
    Domain() = delete;
    Domain(const Domain&) = delete;

    static Domain* Main();

    void AttachThread();
};

struct Field {
    Field() = delete;
    Field(const Field&) = delete;

    int32_t Offset();
    void GetStaticValue(void* pValue);
    void SetStaticValue(void* pValue);
};

struct Method {
    Method() = delete;
    Method(const Method&) = delete;

    System::Object* Invoke(void* __this, void** ppArgs);
    void* GetAddress();
};

template <typename T>
struct HaxMethod {
    HaxMethod(Method* pMethod) { ptr = orig = (T)pMethod->GetAddress(); pBase = pMethod; }

    inline System::Object* Invoke(void* __this, void** args) { return pBase->Invoke(__this, args); }

    Method* pBase;
    T ptr;
    T orig;
};

struct Thread {
    Thread() = delete;
    Thread(const Thread&) = delete;
};

struct System::Object {
    System::Object() = delete;
    System::Object(const System::Object&) = delete;

    static Object* New(Class* pClass);

    System::Object* Ctor();
    void* Unbox();

    Class** ppClass;
    void* monitor;
};

// Represents .NET Framework System.Array
// May be unsafe when T is not a pointer, but generally it is.
template <class T> 
struct System::Array : System::Object {
    System::Array<T>() = delete;
    System::Array<T>(const System::Array<T>& src) = delete;

    inline T*           begin()                             { return &vector[0]; }
    inline const T*     begin() const                       { return &vector[0]; }
    inline T*           end()                               { return &vector[length]; }
    inline const T*     end() const                         { return &vector[length]; }

    inline T&           operator[](size_t i)                { return vector[i]; }
    inline const T&     operator[](size_t i) const          { return vector[i]; }
    
    static Class*       GetClass();
    static void         Clear(System::Array<T>* pArray, Int32 index, Int32 length);
    static void         Sort(System::Array<T>* pArray, Int32 index, Int32 length);

    void* pBounds;
    size_t length;
    T vector[1];
};

template <class TKey, class TValue>
struct System::Dictionary : System::Object {
    
};

// Represents .NET Framework System.Enum
struct System::Enum : System::Object {
    System::Enum() = delete;
    System::Enum(const System::Enum&) = delete;

    static Class* GetClass();
    static Array<String*>* GetNames(System::Type* pType);
    static Array<Int32>* GetValues(System::Type* pType);
};

template <class T> 
struct System::List : System::Object {
    System::List<T>() = delete;
    System::List<T>(const System::List<T>&) = delete;

    Array<T>* pItems;
    Int32 length;
    Int32 version;
    void* pSyncRoot;
};

struct System::String : System::Object {
    System::String() = delete;
    System::String(const System::String&) = delete;

    static System::String* New(const char* data);

    Int32 length;
    Char chars[1];
};

struct System::Type : System::Object {
    System::Type() = delete;
    System::Type(const System::Type&) = delete;

    ::Type* pType;
};

