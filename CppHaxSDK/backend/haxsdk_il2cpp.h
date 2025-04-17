#pragma once

#include <cstdint>

struct Il2CppObject;
struct Il2CppClass;
struct Il2CppDomain;
struct Il2CppAssembly;
struct Il2CppImage;
struct Il2CppField;
struct Il2CppMethod;
struct Il2CppThread;
struct Il2CppType;
struct Il2CppString;

#ifndef METHOD_PARAMS_H
#define METHOD_PARAMS_H
struct MethodParams
{
    void* BackendMethod;
    void* Address;
    void* Thunk;
};
#endif

using t_il2cpp_object_new                   = Il2CppObject* (*)(Il2CppClass* pClass);
using t_il2cpp_object_unbox                 = void* (*)(Il2CppObject* obj);
using t_il2cpp_runtime_object_init          = void (*)(Il2CppObject* obj);
using t_il2cpp_domain_get                   = Il2CppDomain* (*)();
using t_il2cpp_domain_get_assemblies        = Il2CppAssembly* *(*)(Il2CppDomain* domain, size_t* size);
using t_il2cpp_thread_attach                = Il2CppThread* (*)(Il2CppDomain* domain);
using t_il2cpp_assembly_get_image           = Il2CppImage* (*)(Il2CppAssembly* assembly);
using t_il2cpp_class_from_name              = Il2CppClass* (*)(Il2CppImage* image, const char* namespaze, const char* name);
using t_il2cpp_class_get_type               = Il2CppType* (*)(Il2CppClass* pClass);
using t_il2cpp_class_get_methods            = Il2CppMethod* (*)(Il2CppClass* pClass, void** ppIter);
using t_il2cpp_type_get_object              = Il2CppType* (*)(Il2CppType* type);
using t_il2cpp_type_get_name                = const char* (*)(Il2CppType* type);
using t_il2cpp_class_get_field_from_name    = Il2CppField* (*)(Il2CppClass* pClass, const char* name);
using t_il2cpp_runtime_invoke               = Il2CppObject* (*)(Il2CppMethod* pMethod, void* obj, void** params, void** exc);
using t_il2cpp_method_get_param_count       = int32_t(*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_param             = Il2CppType* (*)(Il2CppMethod* pMethod, uint32_t index);
using t_il2cpp_method_get_class             = Il2CppClass* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_return_type       = Il2CppType* (*)(Il2CppMethod* pMethod);
using t_il2cpp_string_new                   = Il2CppString * (*)(const char* str);
using t_il2cpp_class_get_static_field_data  = void* (*)(Il2CppClass*);
using t_il2cpp_field_static_get_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_field_static_set_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_thread_detach                = void (*)(Il2CppThread*);

inline t_il2cpp_object_new                  il2cpp_object_new;
inline t_il2cpp_object_unbox                il2cpp_object_unbox;
inline t_il2cpp_runtime_object_init         il2cpp_runtime_object_init;
inline t_il2cpp_domain_get                  il2cpp_domain_get;
inline t_il2cpp_domain_get_assemblies       il2cpp_domain_get_assemblies;
inline t_il2cpp_thread_attach               il2cpp_thread_attach;
inline t_il2cpp_assembly_get_image          il2cpp_assembly_get_image;
inline t_il2cpp_class_from_name             il2cpp_class_from_name;
inline t_il2cpp_class_get_type              il2cpp_class_get_type;
inline t_il2cpp_class_get_methods           il2cpp_class_get_methods;
inline t_il2cpp_type_get_object             il2cpp_type_get_object;
inline t_il2cpp_type_get_name               il2cpp_type_get_name;
inline t_il2cpp_class_get_field_from_name   il2cpp_class_get_field_from_name;
inline t_il2cpp_runtime_invoke              il2cpp_runtime_invoke;
inline t_il2cpp_method_get_param_count      il2cpp_method_get_param_count;
inline t_il2cpp_method_get_param            il2cpp_method_get_param;
inline t_il2cpp_method_get_class            il2cpp_method_get_class;
inline t_il2cpp_method_get_name             il2cpp_method_get_name;
inline t_il2cpp_method_get_return_type      il2cpp_method_get_return_type;
inline t_il2cpp_string_new                  il2cpp_string_new;
inline t_il2cpp_class_get_static_field_data il2cpp_class_get_static_field_data;
inline t_il2cpp_field_static_get_value      il2cpp_field_static_get_value;
inline t_il2cpp_field_static_set_value      il2cpp_field_static_set_value;
inline t_il2cpp_thread_detach               il2cpp_thread_detach;

namespace Il2Cpp
{
    void Initialize();
    void AttachThread();

    bool AssemblyExists(const char* assembly, Il2CppAssembly** out);
    int32_t FindField(const char* assembly, const char* nameSpace, const char* klass, const char* field);
    void* FindStaticField(const char* assembly, const char* nameSpace, const char* klass, const char* field);
    MethodParams FindMethod(const char* assembly, const char* nameSpace, const char* klass, const char* method, const char* sig);
}

struct Il2CppAssembly
{
    Il2CppImage*        Image;
    uint32_t            Token;
    int32_t             ReferencedAssemblyStart;
    int32_t             ReferencedAssemblyCount;
    const char*         Name; // Actually it's Il2CppAssembly but we need only the first field of Il2CppAssembly - name.
};

struct Il2CppField
{
    const char*         Name;
    const Il2CppType*   Type;
    Il2CppClass*        Parent;
    int32_t             Offset; // If offset is -1, then it's thread static
    uint32_t            Token;
};

struct Il2CppType 
{
    void* dData;
    unsigned int        Attrs : 16;
    int                 Type : 8;
    unsigned int        NumMods : 5;
    unsigned int        Byref : 1;
    unsigned int        Pinned : 1;
    unsigned int        Valuetype : 1;
};

struct Il2CppClass 
{
    Il2CppImage*        Image;
    void*               GcDesc;
    const char*         Name;
    const char*         NameSpace;
    Il2CppType          BvalArg;
    Il2CppType          ThisArg;
    void*               __space[15];
    void*               StaticFields;
};

struct Il2CppMethod
{
    void*               Ptr;
};