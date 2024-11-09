#pragma once

struct MonoThread;
struct MonoDomain;
struct MonoAssembly;
struct MonoObject;
struct MonoType;
struct MonoClass;
struct MonoImage;
struct MonoMethod;
struct MonoString;
struct MonoClassField;
struct MonoVTable;
struct MonoArray;

struct Vector3 {
    float x;
    float y;
    float z;
};

namespace mono {
	void Initialize();
}