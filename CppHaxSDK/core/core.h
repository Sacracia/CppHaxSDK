#pragma once
#ifndef HAX_API
#define HAX_API
#endif

#include <stdint.h>
#include <assert.h>

#include "../logger/logger.h"
#include "../gui/gui.h"

struct MonoThread;
struct MonoType;
struct MonoVTable;
struct MonoThreadsSync;
struct MonoJitInfo;
struct MonoError;
struct MonoReflectionType;
struct MonoProperty;
struct MonoImage;
struct MonoClass;

struct Il2CppThread;
struct Il2CppType;
struct Il2CppReflectionType;
struct Il2CppImage;
struct Il2CppClass;
struct Il2CppMethod;
typedef Il2CppClass Il2CppVTable;

struct MonoObject {
    MonoVTable*         vtable;
    MonoThreadsSync*    synchronisation;
};

struct MonoArrayBounds {
    uint64_t length;
    int64_t  lower_bound;
};

template <class T>
struct MonoArray {
    MonoObject          object;
    MonoArrayBounds*    bounds;
    uint64_t            max_length;
    T                   vector[32];
};

struct MonoString {
    static MonoString* create(const char*);
public:
    MonoObject          object;
    int32_t             length;
    wchar_t             chars[32];
};

struct MonoClassField {
    MonoType*           type;
    const char*         name;
    MonoClass*          parent;
    int                 offset;
};

struct MonoMethod {
    void*         ptr();
};

struct MonoAssembly {
    MonoClass*    klass(const char* name_space, const char* name);
};

struct MonoClass {
    static MonoClass*   find(const char* assembly, const char* name_space, const char* name);
    MonoObject*         ctor();
    MonoMethod*         method(const char* signature);
    MonoMethod*         method(const char* name_space, const char* klass, const char* name, const char* params);
    void*               static_field(const char* name);
    MonoClassField*     field(const char* name);
    MonoReflectionType* system_type();
};

struct MonoDomain {
    static MonoDomain*  current();
    //MonoVTable*         vtable(MonoClass* pClass);
    MonoAssembly*       assembly(const char* assembly);
    void                attach_thread();
};

struct Il2CppObject {
    union {
        Il2CppClass* klass;
        Il2CppVTable* vtable;
    } Il2CppClass;
    void* monitor;  
};

struct Il2CppArrayBounds {
    uint64_t length;
    int64_t  lower_bound;
};

struct Il2CppString {
    static Il2CppString* create(const char*);
public:
    Il2CppObject        object;
    int32_t             length;
    wchar_t             chars[32];
};

struct Il2CppClassField {
    const char* name;
    MonoType* type;
    MonoClass* parent;
    int32_t offset;
    uint32_t token;
};

struct Il2CppMethod final {
    void* pointer;
};

struct Il2CppAssembly {
    Il2CppClass*          klass(const char* name_space, const char* name);
};

struct Il2CppClass final {
    static MonoClass*   find(const char* assembly, const char* name_space, const char* name);
    MonoObject*         ctor();
    MonoMethod*         method(const char* signature);
    MonoMethod*         method(const char* name_space, const char* klass, const char* name, const char* params);
    void*               static_field(const char* name);
    MonoClassField*     field(const char* name);
    MonoReflectionType* system_type();
private:
    char                __space[0xB7];
    void*               static_fields;
};

struct Il2CppDomain {
    static Il2CppDomain*  current();
    Il2CppAssembly*       assembly(const char* assembly);
    void                  attach_thread();
};

// Representation of .NET classes from mscorlib
template <typename T>
struct Array {
    void*                 __space[2];
    uint64_t              max_length;
    T                     vector[32];
};

template <class T>
struct List {
    void*               __space[2];
    Array<T>*           items;
    int32_t             size;
    int32_t             version;
    void*               syncRoot;
};

// Unity core classes
struct Object;
struct Vector3;
struct Transfrom;
struct Camera;

namespace HaxSdk {
	void InitializeCore();
}

struct Vector3 {
    static float Distance(Vector3 a, Vector3 b);
public:
    float x;
    float y;
    float z;
};

struct Object {
    static Array<MonoObject*>* FindObjectsOfType(MonoReflectionType* type);
    static MonoObject* FindObjectOfType(MonoReflectionType* type);
    static Array<Il2CppObject*>* FindObjectsOfType(Il2CppReflectionType* type);
    static Il2CppObject* FindObjectOfType(Il2CppReflectionType* type);
};

struct Transform {
    Vector3 get_position();
    void set_position(Vector3 value);
};

struct Component : MonoObject {
    Transform* get_transform();
};

struct Camera {
    static Camera* main();
    Vector3 WorldToScreenPoint(Vector3 position);
};

struct Screen {
    static int width();
    static int height();
};