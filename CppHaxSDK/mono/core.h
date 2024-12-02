#pragma once
#ifndef HAX_API
#define HAX_API
#endif

#include <stdint.h>
#include <assert.h>

#include "../logger/logger.h"
#include "../gui/gui.h"

/*-----------------------------------------------------------------------------
Forward declarations
-----------------------------------------------------------------------------*/
struct MonoThread;
struct MonoType;
struct MonoVTable;
struct MonoThreadsSync;
struct MonoJitInfo;
struct MonoError;
struct MonoReflectionType;
struct MonoProperty;
struct MonoImage;
class  MonoClass;

class MonoObject {
public:
    MonoObject()                    = delete;
    MonoObject(const MonoObject&)   = delete;
    MonoObject(MonoObject&&)        = delete;
    void* operator new(size_t)      = delete;
public:
    MonoObject&         operator=(const MonoObject&)    = delete;
    MonoObject&         operator=(MonoObject&&)         = delete;
private:
    MonoVTable*         vtable;
    MonoThreadsSync*    synchronisation;
};

template <class T>
class MonoArray {
    struct MonoArrayBounds {
        uint64_t length;
        int64_t  lower_bound;
    };
public:
    MonoArray()                 = delete;
    MonoArray(MonoArray const&) = delete;
    MonoArray(MonoArray&&)      = delete;
    void* operator new(size_t)  = delete;
public:
    MonoArray&          operator=(const MonoArray&)     = delete;
    MonoArray&          operator=(MonoArray&&)          = delete;
public:
    inline T&           operator[](size_t index)        { return vector[index]; }
    inline const T&     operator[](size_t index) const  { return vector[index]; }
    inline T&           at(size_t index)                { return vector[index]; }
    inline const T&     at(size_t index) const          { return vector[index]; }
public:
    inline uint64_t     size() const                    { return max_length; }
    inline T*           data()                          { return vector; }
    inline const T&     data() const                    { return vector; }
private:
    MonoObject          object;
    MonoArrayBounds*    bounds;
    uint64_t            max_length;
    T                   vector[32];
};

class MonoString {
public:
    MonoString()                    = delete;
    MonoString(const MonoString&)   = delete;
    MonoString(MonoString&&)        = delete;
    void* operator new(size_t)      = delete;
public:
    MonoString&         operator=(const MonoString&)    = delete;
    MonoString&         operator=(MonoString&&)         = delete;
public:
    static MonoString*  create(const char* content);
    int32_t             length()                        { return _length; }
    wchar_t*            c_str()                         { return chars; }
private:
    MonoObject          object;
    int32_t             _length;
    wchar_t             chars[32];
};

class MonoClassField {
public:
    MonoClassField()                        = delete;
    MonoClassField(const MonoClassField&)   = delete;
    MonoClassField(MonoClassField&&)        = delete;
    void* operator new(size_t)              = delete;
public:
    MonoClassField&     operator=(const MonoClassField&) = delete;
    MonoClassField&     operator=(MonoClassField&&)      = delete;
public:
    inline int          offset() { return _offset; }
    inline const char*  name()   { return _name; }
private:
    MonoType*           type;
    const char*         _name;
    MonoClass*          parent;
    int                 _offset;
};

class MonoMethod {
public:
    MonoMethod()                    = delete;
    MonoMethod(const MonoMethod&)   = delete;
    MonoMethod(MonoMethod&&)        = delete;
    void* operator new(size_t)      = delete;
public:
    MonoMethod& operator=(const MonoMethod&) = delete;
    MonoMethod& operator=(MonoMethod&&)      = delete;
public:
    void*       ptr();
};

class MonoAssembly {
public:
    MonoAssembly()                    = delete;
    MonoAssembly(const MonoAssembly&) = delete;
    MonoAssembly(MonoAssembly&&)      = delete;
    void* operator new(size_t)        = delete;
public:
    MonoAssembly& operator=(const MonoAssembly&) = delete;
    MonoAssembly& operator=(MonoAssembly&&)      = delete;
public:
    MonoClass*    klass(const char* name_space, const char* name);
};

struct MonoMethodWrapper {
    MonoMethod* pMethod;
    void* ptr;
};

class MonoClass {
public:
    MonoClass()                 = delete;
    MonoClass(const MonoClass&) = delete;
    MonoClass(MonoClass&&)      = delete;
    void* operator new(size_t)  = delete;
public:
    MonoClass&          operator=(const MonoClass&) = delete;
    MonoClass&          operator=(MonoClass&&)      = delete;
public:
    static MonoClass*   find(const char* assembly, const char* name_space, const char* name);
    MonoMethodWrapper   method(const char* signature);
    MonoMethodWrapper   method(const char* name_space, const char* klass, const char* name, const char* params);
    void*               static_field(const char* name);
    MonoClassField*     field(const char* name);
    MonoReflectionType* system_type();
};

class MonoDomain {
public:
    MonoDomain()                  = delete;
    MonoDomain(const MonoDomain&) = delete;
    MonoDomain(MonoDomain&&)      = delete;
    void* operator new(size_t)    = delete;
public:
    MonoDomain&         operator=(const MonoDomain&) = delete;
    MonoDomain&         operator=(MonoDomain&&)      = delete;
public:
    static MonoDomain*  current();
    MonoVTable*         vtable(MonoClass* pClass);
    MonoAssembly*       assembly(const char* assembly);
    void                attach_thread();
};

// Representation of .NET classes from mscorlib
using String = MonoString;
using Type = MonoReflectionType;
template <typename T> using Array = MonoArray<T>;
template <typename T> struct List;

// Unity core classes
struct Object;
struct Vector3;
struct Transfrom;
struct Camera;

namespace HaxSdk {
	void InitializeMono();
}

template <class T>
struct List {
    MonoObject          object;
    Array<T>*           _items;
    int32_t             _size;
    int32_t             _version;
    void*               _syncRoot;
};

struct Vector3 {
    static float Distance(Vector3 a, Vector3 b);
public:
    float x;
    float y;
    float z;
};

struct Object {
    static Array<MonoObject*>* FindObjectsOfType(Type* type);
    static MonoObject* FindObjectOfType(Type* type);
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