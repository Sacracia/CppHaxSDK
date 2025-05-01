#include "haxsdk_il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <format>
#include <unordered_map>

#include "../haxsdk.h"
#include "../haxsdk_settings.h"

#define GET_BACKEND_API(n)      n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n));\
                                HAX_ASSERT(n, #n)
#define GET_BACKEND_API_OPT(n)  n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n))

#define FIELD_ATTRIBUTE_STATIC  0x10

using t_il2cpp_object_new                   = Il2CppObject* (*)(Il2CppClass* pClass);
using t_il2cpp_object_unbox                 = void* (*)(Il2CppObject* obj);
using t_il2cpp_runtime_object_init          = void (*)(Il2CppObject* obj);
using t_il2cpp_domain_get                   = Il2CppDomain* (*)();
using t_il2cpp_domain_get_assemblies        = Il2CppAssembly** (*)(Il2CppDomain* domain, size_t* size);
using t_il2cpp_thread_attach                = Il2CppThread* (*)(Il2CppDomain* domain);
using t_il2cpp_assembly_get_image           = Il2CppImage* (*)(Il2CppAssembly* assembly);
using t_il2cpp_class_from_name              = Il2CppClass* (*)(Il2CppImage* image, const char* namespaze, const char* name);
using t_il2cpp_class_get_type               = Il2CppType* (*)(Il2CppClass* pClass);
using t_il2cpp_class_get_methods            = Il2CppMethodInfo * (*)(Il2CppClass* pClass, void** ppIter);
using t_il2cpp_type_get_object              = Il2CppReflectionType* (*)(Il2CppType* type);
using t_il2cpp_type_get_name                = const char* (*)(Il2CppType* type);
using t_il2cpp_class_get_field_from_name    = Il2CppFieldInfo* (*)(Il2CppClass* pClass, const char* name);
using t_il2cpp_runtime_invoke               = Il2CppObject* (*)(Il2CppMethodInfo* pMethod, void* obj, void** params, Il2CppException** exc);
using t_il2cpp_method_get_param_count       = int32_t(*)(Il2CppMethodInfo* pMethod);
using t_il2cpp_method_get_param             = Il2CppType* (*)(Il2CppMethodInfo* pMethod, uint32_t index);
using t_il2cpp_method_get_class             = Il2CppClass* (*)(Il2CppMethodInfo* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(Il2CppMethodInfo* pMethod);
using t_il2cpp_method_get_return_type       = Il2CppType* (*)(Il2CppMethodInfo* pMethod);
using t_il2cpp_string_new                   = Il2CppString* (*)(const char* str);
using t_il2cpp_class_get_static_field_data  = void* (*)(Il2CppClass*);
using t_il2cpp_field_static_get_value       = void (*)(Il2CppFieldInfo* pField, void* pValue);
using t_il2cpp_field_static_set_value       = void (*)(Il2CppFieldInfo* pField, void* pValue);
using t_il2cpp_thread_detach                = void (*)(Il2CppThread*);
using t_il2cpp_image_get_name               = const char* (*)(Il2CppImage*);
using t_il2cpp_domain_assembly_open         = Il2CppAssembly* (*)(Il2CppDomain* domain, const char* name);
using t_il2cpp_class_from_system_type       = Il2CppClass* (*)(Il2CppReflectionType*);
using t_il2cpp_get_corlib                   = Il2CppImage* (*)();
using t_il2cpp_image_get_assembly           = Il2CppAssembly* (*)(Il2CppImage*);
using t_il2cpp_value_box                    = Il2CppObject* (*)(Il2CppClass*, void*);

struct Il2CppObject
{
    union
    {
        Il2CppClass* Klass;
        Il2CppVTable* Vtable;
    };
    MonitorData* Monitor;
};

struct Il2CppDomain
{
    Il2CppAppDomain* Domain;
    Il2CppAppDomainSetup* Setup;
    Il2CppAppContext* DefaultContext;
    const char* FriendlyName;
    // ...
};

struct Il2CppAssemblyName
{
    const char* Name;
    const char* Culture;
    const char* HashValue;
    const char* PublicKey;
    // ...
};

struct Il2CppAssembly
{
    Il2CppImage* Image;
    uint32_t Token;
    int32_t ReferencedAssemblyStart;
    int32_t ReferencedAssemblyCount;
    Il2CppAssemblyName AName;
};

struct Il2CppString : Il2CppObject
{
    int32_t length;                             ///< Length of string *excluding* the trailing null (which is included in 'chars').
    wchar_t chars[1];
};

struct Il2CppException : Il2CppObject
{
#if !defined(NET_4_0)
    /* Stores the IPs and the generic sharing infos
       (vtable/MRGCTX) of the frames. */
    Il2CppArray* trace_ips;
    Il2CppException* inner_ex;
    Il2CppString* message;
    Il2CppString* help_link;
    Il2CppString* class_name;
    Il2CppString* stack_trace;
#else
    Il2CppString* className;
    Il2CppString* message;
    Il2CppObject* _data;
    Il2CppException* inner_ex;
    Il2CppString* _helpURL;
    Il2CppArray<void*>* trace_ips;
    Il2CppString* stack_trace;
#endif
};

struct Il2CppType
{
    union
    {
        int32_t klassIndex; /* for VALUETYPE and CLASS */
        const Il2CppType* type;   /* for PTR and SZARRAY */
        Il2CppArrayType* array; /* for ARRAY */
        //MonoMethodSignature *method;
        int32_t genericParameterIndex; /* for VAR and MVAR */
        Il2CppGenericClass* generic_class; /* for GENERICINST */
    };
    unsigned int attrs : 16; /* param attributes or field flags */
    /*...*/
};

struct Il2CppFieldInfo
{
    const char* name;
    const Il2CppType* type;
    Il2CppClass* parent;
    int32_t offset; // If offset is -1, then it's thread static
    uint32_t token;
};

struct Il2CppMethodInfo
{
    void* methodPointer;
    /*...*/
};

static t_il2cpp_object_new                  il2cpp_object_new;
static t_il2cpp_object_unbox                il2cpp_object_unbox;
static t_il2cpp_runtime_object_init         il2cpp_runtime_object_init;
static t_il2cpp_domain_get                  il2cpp_domain_get;
static t_il2cpp_domain_get_assemblies       il2cpp_domain_get_assemblies;
static t_il2cpp_thread_attach               il2cpp_thread_attach;
static t_il2cpp_assembly_get_image          il2cpp_assembly_get_image;
static t_il2cpp_class_from_name             il2cpp_class_from_name;
static t_il2cpp_class_get_type              il2cpp_class_get_type;
static t_il2cpp_class_get_methods           il2cpp_class_get_methods;
static t_il2cpp_type_get_object             il2cpp_type_get_object;
static t_il2cpp_type_get_name               il2cpp_type_get_name;
static t_il2cpp_class_get_field_from_name   il2cpp_class_get_field_from_name;
static t_il2cpp_runtime_invoke              il2cpp_runtime_invoke;
static t_il2cpp_method_get_param_count      il2cpp_method_get_param_count;
static t_il2cpp_method_get_param            il2cpp_method_get_param;
static t_il2cpp_method_get_class            il2cpp_method_get_class;
static t_il2cpp_method_get_name             il2cpp_method_get_name;
static t_il2cpp_method_get_return_type      il2cpp_method_get_return_type;
static t_il2cpp_string_new                  il2cpp_string_new;
static t_il2cpp_class_get_static_field_data il2cpp_class_get_static_field_data;
static t_il2cpp_field_static_get_value      il2cpp_field_static_get_value;
static t_il2cpp_field_static_set_value      il2cpp_field_static_set_value;
static t_il2cpp_thread_detach               il2cpp_thread_detach;
static t_il2cpp_image_get_name              il2cpp_image_get_name;
static t_il2cpp_domain_assembly_open        il2cpp_domain_assembly_open;
static t_il2cpp_class_from_system_type      il2cpp_class_from_system_type;
static t_il2cpp_get_corlib                  il2cpp_get_corlib;
static t_il2cpp_image_get_assembly          il2cpp_image_get_assembly;
static t_il2cpp_value_box                   il2cpp_value_box;

static std::unordered_map<Il2CppReflectionType*, Il2CppClass*> g_TypeToClass;

namespace Il2Cpp
{
    void Initialize()
    {
        HMODULE handle = (HMODULE)HaxSdk::GetGlobals().m_BackendHandle;
        GET_BACKEND_API(il2cpp_object_new);
        GET_BACKEND_API(il2cpp_object_unbox);
        GET_BACKEND_API(il2cpp_runtime_object_init);
        GET_BACKEND_API(il2cpp_domain_get);
        GET_BACKEND_API(il2cpp_domain_get_assemblies);
        GET_BACKEND_API(il2cpp_thread_attach);
        GET_BACKEND_API(il2cpp_assembly_get_image);
        GET_BACKEND_API(il2cpp_class_from_name);
        GET_BACKEND_API(il2cpp_class_get_type);
        GET_BACKEND_API(il2cpp_class_get_methods);
        GET_BACKEND_API(il2cpp_type_get_object);
        GET_BACKEND_API(il2cpp_type_get_name);
        GET_BACKEND_API(il2cpp_class_get_field_from_name);
        GET_BACKEND_API(il2cpp_runtime_invoke);
        GET_BACKEND_API(il2cpp_method_get_param_count);
        GET_BACKEND_API(il2cpp_method_get_param);
        GET_BACKEND_API(il2cpp_method_get_class);
        GET_BACKEND_API(il2cpp_method_get_name);
        GET_BACKEND_API(il2cpp_method_get_return_type);
        GET_BACKEND_API(il2cpp_string_new);
        GET_BACKEND_API(il2cpp_field_static_get_value);
        GET_BACKEND_API(il2cpp_field_static_set_value);
        GET_BACKEND_API_OPT(il2cpp_class_get_static_field_data);
        GET_BACKEND_API(il2cpp_thread_detach);
        GET_BACKEND_API(il2cpp_image_get_name);
        GET_BACKEND_API(il2cpp_domain_assembly_open);
        GET_BACKEND_API(il2cpp_class_from_system_type);
        GET_BACKEND_API(il2cpp_get_corlib);
        GET_BACKEND_API(il2cpp_image_get_assembly);
        GET_BACKEND_API(il2cpp_value_box);
    }

    void AttachThread()
    {
        il2cpp_thread_attach(il2cpp_domain_get());
    }

    Il2CppDomain* GetDomain()
    {
        return il2cpp_domain_get();
    }

    Il2CppImage* GetCoreLib()
    {
        return il2cpp_get_corlib();
    }

    Il2CppImage* GetUnityCoreLib()
    {
        Il2CppDomain* domain = il2cpp_domain_get();
        Il2CppAssembly* assembly = il2cpp_domain_assembly_open(domain, "UnityEngine.CoreModule");
        if (assembly)
            return il2cpp_assembly_get_image(assembly);

        assembly = il2cpp_domain_assembly_open(domain, "UnityEngine");
        HAX_ASSERT(assembly, "Unable to get unity core assembly");
        return il2cpp_assembly_get_image(assembly);
    }

    Il2CppImage* LoadAssembly(Il2CppDomain* domain, const char* name)
    {
        Il2CppAssembly* assembly = il2cpp_domain_assembly_open(domain, name);
        return assembly ? il2cpp_assembly_get_image(assembly) : nullptr;
    }

    const char* GetFriendlyName(Il2CppDomain* domain)
    {
        return domain->FriendlyName;
    }

    Il2CppAssemblyName* AssemblyGetName(Il2CppImage* assembly)
    {
        return &il2cpp_image_get_assembly(assembly)->AName;
    }

    const char* AssemblyNameGetName(Il2CppAssemblyName* assemblyName)
    {
        return assemblyName->Name;
    }

    Il2CppReflectionType* AssemblyGetType(Il2CppImage* image, const char* nameSpace, const char* name)
    {
        Il2CppClass* klass = il2cpp_class_from_name(image, nameSpace, name);
        if (klass)
        {
            Il2CppReflectionType* type = il2cpp_type_get_object(il2cpp_class_get_type(klass));
            g_TypeToClass[type] = klass;
            return type;
        }
        return nullptr;
    }

    Il2CppFieldInfo* GetField(Il2CppReflectionType* type, const char* name)
    {
        return il2cpp_class_get_field_from_name(il2cpp_class_from_system_type(type), name);
    }

    static void GetMethodSignature(Il2CppMethodInfo* method, char* buff, size_t buffSize)
    {
        strcpy_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_return_type(method)));
        strcat_s(buff, buffSize, "(");
        uint32_t nParams = il2cpp_method_get_param_count(method);
        if (nParams > 0)
        {
            strcat_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_param(method, 0)));
            for (uint32_t i = 1; i < nParams; ++i)
            {
                strcat_s(buff, buffSize, ",");
                strcat_s(buff, buffSize, il2cpp_type_get_name(il2cpp_method_get_param(method, i)));
            }
        }
        strcat_s(buff, buffSize, ")");
    }

    Il2CppMethodInfo* GetMethod(Il2CppReflectionType* type, const char* name, const char* sig)
    {
        void* iter = nullptr;
        Il2CppClass* klass = g_TypeToClass[type];
        HAX_ASSERT(klass, "Type-to-class cache is corrupted!");

        while (Il2CppMethodInfo* method = il2cpp_class_get_methods(klass, &iter))
        {
            const char* methodName = il2cpp_method_get_name(method);
            if (strcmp(methodName, name) == 0)
            {
                if (!sig || !sig[0])
                    return method;

                char buff[256] = { 0 };
                GetMethodSignature(method, buff, sizeof(buff));
                if (strcmp(buff, sig) == 0)
                    return method;
            }
        }
        return nullptr;
    }

    void GetStaticValue(Il2CppFieldInfo* field, void* value)
    {
        il2cpp_field_static_get_value(field, value);
    }

    Il2CppObject* Invoke(Il2CppMethodInfo* method, void* __this, void** args, Il2CppException** ex)
    {
        return il2cpp_runtime_invoke(method, __this, args, ex);
    }

    wchar_t* StringGetChars(Il2CppString* str)
    {
        return str->chars;
    }

    int32_t StringGetLength(Il2CppString* str)
    {
        return str->length;
    }

    const char* ExceptionGetMessage(Il2CppException* ex)
    {
        return HaxSdk::ToUTF8(ex->message->chars, ex->message->length);
    }

    const char* ExceptionGetStacktrace(Il2CppException* ex)
    {
        return ex->stack_trace ? HaxSdk::ToUTF8(ex->stack_trace->chars, ex->stack_trace->length) : nullptr;
    }

    int32_t GetFieldOffset(Il2CppFieldInfo* field)
    {
        return field->offset;
    }

    void* GetStaticFieldAddress(Il2CppFieldInfo* field)
    {
        if (field->type->attrs & FIELD_ATTRIBUTE_STATIC && field->offset >= 0)
            return (char*)il2cpp_class_get_static_field_data(field->parent) + field->offset;

        return nullptr;
    }

    void* GetFunctionPointer(Il2CppMethodInfo* method)
    {
        return method->methodPointer;
    }

    void* Unbox(Il2CppObject* obj)
    {
        return il2cpp_object_unbox(obj);
    }

    Il2CppReflectionType* ObjectGetType(Il2CppObject* obj)
    {
        Il2CppReflectionType* type = il2cpp_type_get_object(il2cpp_class_get_type(obj->Klass));
        g_TypeToClass[type] = obj->Klass;
        return type;
    }

    Il2CppObject* NewObject(Il2CppReflectionType* type)
    {
        HAX_ASSERT(type, "NewObject type is null");
        Il2CppObject* object = il2cpp_object_new(il2cpp_class_from_system_type(type));
        il2cpp_runtime_object_init(object);
        return object;
    }

    Il2CppObject* AllocObject(Il2CppReflectionType* type)
    {
        HAX_ASSERT(type, "NewObject type is null");
        Il2CppClass* klass = g_TypeToClass[type];
        HAX_ASSERT(klass, "Type-to-class cache is corrupted!");
        return il2cpp_object_new(klass);
    }

    Il2CppString* NewString(const char* text)
    {
        return il2cpp_string_new(text);
    }

    Il2CppObject* BoxValue(Il2CppReflectionType* type, void* value)
    {
        return il2cpp_value_box(g_TypeToClass[type], value);
    }
}