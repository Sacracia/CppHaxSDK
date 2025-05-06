#pragma once

#include <cstdint>

struct Il2CppImage;
struct Il2CppDomain;
struct Il2CppThread;
struct Il2CppString;
struct Il2CppException;
struct Il2CppVTable;
struct MonitorData;
struct Il2CppType;
struct Il2CppClass;
struct Il2CppAssembly;
struct Il2CppObject;
struct Il2CppAppDomain;
struct Il2CppMarshalByRefObject;
struct Il2CppAppDomainSetup;
struct Il2CppAppContext;
struct Il2CppAssemblyName;
struct Il2CppReflectionType;
struct Il2CppFieldInfo; // renamed from FieldInfo
struct Il2CppMethodInfo; // renamed from MethodInfo
struct Il2CppArrayBounds;
struct Il2CppArrayType;
struct Il2CppTypeDefinitionIndex;
struct Il2CppGenericClass;
struct Il2CppExceptionWrapper
{
    Il2CppException* ex;
};

template <typename T>
struct Il2CppArray
{
    void* __space[2];
    Il2CppArrayBounds* bounds;
    size_t max_length;
    __declspec(align(8)) T vector[1];
};

namespace Il2Cpp
{
    void Initialize();
    void AttachThread();
    Il2CppDomain* GetDomain();
    Il2CppImage* GetCoreLib();
    Il2CppImage* GetUnityCoreLib();
    Il2CppImage* LoadAssembly(Il2CppDomain* domain, const char* name);
    const char* GetFriendlyName(Il2CppDomain* domain);
    Il2CppAssemblyName* AssemblyGetName(Il2CppImage* assembly);
    const char* AssemblyNameGetName(Il2CppAssemblyName* assemblyName);
    Il2CppReflectionType* AssemblyGetType(Il2CppImage* assembly, const char* nameSpace, const char* name);
    Il2CppFieldInfo* GetField(Il2CppReflectionType* type, const char* name);
    Il2CppMethodInfo* GetMethod(Il2CppReflectionType* type, const char* name, const char* sig);
    void GetStaticValue(Il2CppFieldInfo* field, void* value);
    Il2CppObject* Invoke(Il2CppMethodInfo* method, void* __this, void** args, Il2CppException** ex);
    wchar_t* StringGetChars(Il2CppString* str);
    int32_t StringGetLength(Il2CppString* str);
    const char* ExceptionGetMessage(Il2CppException* ex);
    const char* ExceptionGetStacktrace(Il2CppException* ex);
    int32_t GetFieldOffset(Il2CppFieldInfo* field);
    void* GetStaticFieldAddress(Il2CppFieldInfo* field);
    void* GetFunctionPointer(Il2CppMethodInfo* method);
    void* Unbox(Il2CppObject* obj);
    Il2CppReflectionType* ObjectGetType(Il2CppObject* obj);
    Il2CppObject* NewObject(Il2CppReflectionType* type);
    Il2CppObject* AllocObject(Il2CppReflectionType* type);
    Il2CppString* NewString(const char* text);
    Il2CppObject* BoxValue(Il2CppReflectionType* type, void* value);
}