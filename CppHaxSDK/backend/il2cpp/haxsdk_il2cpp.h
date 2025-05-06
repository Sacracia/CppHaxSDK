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
struct Il2CppField; // renamed from FieldInfo
struct Il2CppMethod; // renamed from MethodInfo
struct Il2CppArrayBounds;
struct Il2CppArrayType;
struct Il2CppTypeDefinitionIndex;
struct Il2CppGenericClass;
struct Il2CppExceptionWrapper;
struct Il2CppArray;

namespace il2cpp
{
    void Initialize();

    struct Assembly
    {
        static Il2CppImage* GetImage(Il2CppAssembly* assembly);
        static Il2CppAssembly* Load(Il2CppDomain* domain, const char* name);
    };

    struct Class
    {
        static Il2CppClass* FromName(Il2CppImage* image, const char* nameSpace, const char* name);
        static Il2CppClass* FromSystemType(Il2CppReflectionType* type);
        static Il2CppField* GetFieldFromName(Il2CppClass* klass, const char* name);
        static Il2CppMethod* GetMethodFromName(Il2CppClass* klass, const char* name, const char* signature);
        //static Il2CppMethod* GetMethods(Il2CppClass* klass, void** iter);
        static void* GetStaticFieldData(Il2CppClass* klass);
        static Il2CppType* GetType(Il2CppClass* klass);
    };

    struct Domain
    {
        static Il2CppDomain* GetCurrent();
    };

    struct Exception
    {
        static Il2CppString* GetMessage(Il2CppException* ex);
        static Il2CppException* GetNullReferenceException();
        static Il2CppException* GetArgumentOutOfRangeException();
    };

    struct Field
    {
        static bool IsStatic(Il2CppField* field);
        static size_t GetOffset(Il2CppField* field);
        static Il2CppClass* GetParent(Il2CppField* field);
        static void* GetAddress(Il2CppField* field, Il2CppObject* __this);
        static void StaticGetValue(Il2CppField* field, void* value);
        static void StaticSetValue(Il2CppField* field, void* value);
    };

    struct GCHandle
    {
        static uint32_t New(Il2CppObject* obj, bool pinned);
        static uint32_t NewWeakRef(Il2CppObject* obj, bool trackResurrection);
        static Il2CppObject* GetTaget(uint32_t handle);
        static void Free(uint32_t handle);
    };

    struct ReflectionType
    {
        static Il2CppClass* GetClass(Il2CppReflectionType* type);
    };

    struct Image
    {
        static Il2CppImage* GetCorlib();
        static Il2CppAssembly* GetAssembly(Il2CppImage* image);
        static const char* GetName(Il2CppImage* image);
    };

    struct Method
    {
        static Il2CppClass* GetClass(Il2CppMethod* method);
        static const char* GetName(Il2CppMethod* method);
        static Il2CppType* GetParam(Il2CppMethod* method, uint32_t index);
        static int32_t GetParamCount(Il2CppMethod* method);
        static Il2CppType* GetReturnType(Il2CppMethod* method);
    };

    struct Object
    {
        static Il2CppObject* Box(Il2CppClass* klass, void* data);
        static Il2CppObject* New(Il2CppClass* klass);
        static Il2CppClass* GetClass(Il2CppObject* object);
        static void* Unbox(Il2CppObject* object);
    };

    struct Reflection
    {
        static Il2CppReflectionType* TypeGetObject(Il2CppType* type);
    };

    struct Runtime
    {
        static Il2CppObject* Invoke(Il2CppMethod* method, void* __this, void** args, Il2CppException** ex);
        static void ObjectInit(Il2CppObject* object);
    };

    struct String
    {
        static Il2CppString* New(const char* str);
    };

    struct Thread
    {
        static Il2CppThread* Attach(Il2CppDomain* domain);
        static void Detach(Il2CppThread* thread);
    };

    struct Type
    {
        static const char* GetName(Il2CppType* type);
    };
}