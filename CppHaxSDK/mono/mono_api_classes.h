#pragma once

#include <stdint.h>

struct MonoThread;
struct MonoDomain;
struct MonoAssembly;
struct MonoType;
struct MonoClass;
struct MonoImage;
struct MonoMethod;
struct MonoString;
struct MonoVTable;
struct MonoArray;
struct MonoThreadsSync;
struct MonoJitInfo;
struct MonoError;

struct MonoObject {
private:
    MonoObject();
public:
    MonoMethod* GetMonoMethod(void* addr);
public:
    MonoVTable* vtable;
    MonoThreadsSync* synchronisation;
};

struct MonoClassField {
    MonoType* type;
    const char* name;
    MonoClass* parent;
    int offset;
};

typedef struct {
    uint64_t length;
    int64_t lower_bound;
} MonoArrayBounds;

struct Vector3 {
    float x;
    float y;
    float z;
};

struct String {
    MonoObject object;
    int32_t length;
    wchar_t chars[32];
};

template <class T>
struct Array {
    MonoObject object;
    MonoArrayBounds* bounds;
    int max_length;
    T vector[32];
};

template <class T>
struct List {
    MonoObject object;
    Array<T>* _items;
    int32_t _size;
    int32_t _version;
    void* _syncRoot;
};