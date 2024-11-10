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

struct MonoClassField {
    MonoType* type;
    const char* name;
    MonoClass* parent;
    int         offset;
};

struct MonoVTable;
struct MonoArray;

struct Vector3 {
    float x;
    float y;
    float z;
};

class HeroController {
    //static MonoClass* __class;
public:
    MonoObject* playerData();
};

namespace mono {
	void Initialize();
}