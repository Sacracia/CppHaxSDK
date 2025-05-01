#pragma once

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

struct MonoObject
{
    MonoVTable* Vtable;
    MonoThreadsSync* Synchronisation;
};

template <typename T>
struct MonoArray
{
    void* space[2];
    MonoArrayBounds* bounds;
    size_t max_length;
    __declspec(align(8)) T vector[1];
};

template <typename T>
struct MonoList
{
    MonoArray<T>* arr;
    int32_t               length;
    int32_t               version;
    void* syncroot;
};

namespace Mono
{
    void Initialize();
    void AttachThread();
    MonoDomain* GetDomain();
    MonoImage* GetCoreLib();
    MonoImage* GetUnityCoreLib();
    MonoImage* LoadAssembly(MonoDomain* domain, const char* name);
    //const char* GetFriendlyName(void* domain);
    MonoAssemblyName* AssemblyGetName(MonoImage* image);
    const char* AssemblyNameGetName(MonoAssemblyName* assemblyName);
    MonoReflectionType* AssemblyGetType(MonoImage* assembly, const char* nameSpace, const char* name);
    MonoClassField* GetField(MonoReflectionType* type, const char* name);
    MonoMethod* GetMethod(MonoReflectionType* type, const char* name, const char* sig);
    void GetStaticValue(MonoClassField* field, void* value);
    MonoObject* Invoke(MonoMethod* method, void* __this, void** args, MonoException** ex);
    wchar_t* StringGetChars(MonoString* str);
    int32_t StringGetLength(MonoString* str);
    //const char* StringToUTF8(MonoString*);
    const char* ExceptionGetMessage(MonoException* ex);
    const char* ExceptionGetStacktrace(MonoException* ex);
    int32_t GetFieldOffset(MonoClassField* field);
    void* GetStaticFieldAddress(MonoClassField* field);
    void* GetFunctionPointer(MonoMethod* method);
    void* Unbox(MonoObject* obj);
    void* GetUnmanagedThunk(MonoMethod* method);
    MonoReflectionType* ObjectGetType(MonoObject* obj);
    MonoObject* NewObject(MonoReflectionType* type);
    MonoObject* AllocObject(MonoReflectionType* type);
    MonoString* NewString(const char* text);
    MonoObject* BoxValue(MonoReflectionType* type, void* value);
}