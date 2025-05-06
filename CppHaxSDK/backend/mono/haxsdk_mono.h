#pragma once

#undef GetMessage

#include <cstdint>

struct MonoAssembly;
struct MonoAssemblyName;
struct MonoClass;
struct MonoDomain;
struct MonoType;
struct MonoImage;
struct MonoMethod;
struct MonoObject;
struct MonoString;
struct MonoThread;
struct MonoVTable;
struct MonoMethodSignature;
struct MonoException;
struct MonoThreadsSync;
struct MonoAppDomainSetup;
struct MonoAppContext;
struct MonoGHashTable;
struct MonoAppDomain;
struct MonoReflectionType;
struct MonoArrayType;
struct MonoGenericParam;
struct MonoGenericClass;
struct MonoClassField;
struct MonoExceptionWrapper;
struct MonoArrayBounds;
struct MonoArray;

namespace mono
{
    void Initialize();

    struct Assembly
    {
        //static void Foreach(GFunc fn, void* data);
        static MonoImage* GetImage(MonoAssembly* assembly);
        static MonoAssembly* Load(MonoDomain* domain, const char* name);
    };

    struct Class
    {
        static MonoClass* FromName(MonoImage* image, const char* nameSpace, const char* name);
        static MonoClassField* GetFieldFromName(MonoClass* klass, const char* name);
        static MonoMethod* GetMethodFromName(MonoClass* klass, const char* name, const char* signature);
        static const char* GetName(MonoClass* klass);
        static const char* GetNamespace(MonoClass* klass);
        static MonoType* GetType(MonoClass* klass);
        static MonoVTable* VTable(MonoDomain* domain, MonoClass* klass);
    };

    struct Domain
    {
        static MonoDomain* GetCurrent();
        static MonoDomain* GetRoot();
    };

    struct Exception
    {
        static MonoString* GetMessage(MonoException* ex);
        static MonoException* GetNullReferenceException();
        static MonoException* GetArgumentOutOfRangeException();
    };

    struct Field
    {
        static bool IsStatic(MonoClassField* field);
        static MonoClass* GetParent(MonoClassField* field);
        static void* GetAddress(MonoClassField* field, MonoObject* __this);
        static void StaticGetValue(MonoVTable* vtable, MonoClassField* field, void* value);
        static void StaticSetValue(MonoVTable* vtable, MonoClassField* field, void* value);
    };

    struct GCHandle
    {
        static uint32_t New(MonoObject* obj, bool pinned);
        static uint32_t NewWeakRef(MonoObject* obj, bool trackResurrection);
        static MonoObject* GetTaget(uint32_t handle);
        static void Free(uint32_t handle);
    };

    struct Image
    {
        static MonoImage* GetCorlib();
        static MonoAssembly* GetAssembly(MonoImage* image);
        const char* GetName(MonoImage* image);
    };

    struct Method
    {
        static void* Compile(MonoMethod* method);
        static const char* GetName(MonoMethod* method);
        static void* GetUnmanagedThunk(MonoMethod* method);
        static MonoMethodSignature* Signature(MonoMethod* method);
    };

    struct Object
    {
        static MonoObject* Box(MonoDomain* domain, MonoClass* klass, void* data);
        static MonoClass* GetClass(MonoObject* object);
        static MonoObject* New(MonoDomain* domain, MonoClass* klass);
        static void* Unbox(MonoObject* object);
    };

    struct ReflectionType
    {
        static MonoClass* GetClass(MonoReflectionType* type);
    };

    struct Runtime
    {
        static MonoObject* Invoke(MonoMethod* method, void* __this, void** args, MonoException** ex);
        static void ObjectInit(MonoObject* object);
    };

    struct Signature
    {
        static uint32_t GetParamCount(MonoMethodSignature* methodSig);
        static MonoType* GetParams(MonoMethodSignature* methodSig, void** iter);
        static MonoType* GetReturnType(MonoMethodSignature* methodSig);
    };

    struct String
    {
        static MonoString* New(MonoDomain* domain, const char* str);
    };

    struct Thread
    {
        static MonoThread* Attach(MonoDomain* domain);
        static MonoThread* Current();
        static void Detach(MonoThread* thread);
    };

    struct Type
    {
        static char* GetName(MonoType* type);
    };

    struct Reflection
    {
        static MonoReflectionType* TypeGetObject(MonoDomain* domain, MonoType* type);
    };

    struct VTable
    {
        static void* GetStaticFieldData(MonoVTable* vtable);
    };
}