#pragma once

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

struct MonoObject {
};

struct MonoClassField {
    MonoType* type;
    const char* name;
    MonoClass* parent;
    int         offset;
};