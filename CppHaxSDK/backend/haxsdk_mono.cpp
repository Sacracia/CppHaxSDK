#include "haxsdk_mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <format>
#include <iostream>
#include <unordered_map>

#include "../haxsdk.h"
#include "../haxsdk_settings.h"

#define GET_BACKEND_API(n)      n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n));\
                                HAX_ASSERT(n, #n)
#define GET_BACKEND_API_OPT(n)  n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n))

#define FIELD_ATTRIBUTE_STATIC 0x10

using GFunc                                 = void(__cdecl*)(void* data, void* user_data);
using t_mono_get_root_domain                = MonoDomain* (*)();
using t_mono_domain_assembly_open           = MonoAssembly* (*)(MonoDomain*, const char*);
using t_mono_thread_attach                  = MonoThread* (*)(MonoDomain*);
using t_mono_domain_get                     = MonoDomain* (*)();
using t_mono_string_new                     = MonoString* (*)(MonoDomain*, const char*);
using t_mono_assembly_get_image             = MonoImage* (*)(MonoAssembly*);
using t_mono_assembly_foreach               = void (*)(GFunc, void*);
using t_mono_class_from_name                = MonoClass* (*)(MonoImage*, const char*, const char*);
using t_mono_class_get_methods              = MonoMethod* (*)(MonoClass*, void**);
using t_mono_class_get_field_from_name      = MonoClassField* (*)(MonoClass*, const char*);
using t_mono_class_vtable                   = MonoVTable* (*)(MonoDomain*, MonoClass*);
using t_mono_class_get_namespace            = const char* (*)(MonoClass*);
using t_mono_class_get_name                 = const char* (*)(MonoClass*);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
using t_mono_class_get_type                 = MonoType* (*)(MonoClass*);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
using t_mono_method_signature               = MonoMethodSignature* (*)(MonoMethod*);
using t_mono_method_get_name                = const char* (*)(MonoMethod*);
using t_mono_compile_method                 = void* (*)(MonoMethod*);
using t_mono_jit_info_table_find            = void* (*)(MonoDomain*, void*);
using t_mono_jit_info_get_method            = MonoMethod* (*)(void*);
using t_mono_runtime_invoke                 = MonoObject* (*)(MonoMethod*, void*, void**, MonoException**);
using t_mono_print_unhandled_exception      = void (*)(MonoObject*);
using t_mono_object_unbox                   = void* (*)(MonoObject*);
using t_mono_vtable_get_static_field_data   = void* (*)(MonoVTable*);
using t_mono_type_get_object                = MonoReflectionType* (*)(MonoDomain*, MonoType*);
using t_mono_object_new                     = MonoObject * (*)(MonoDomain*, MonoClass*);
using t_mono_runtime_object_init            = void (*)(MonoObject*);
using t_mono_signature_get_return_type      = MonoType* (*)(MonoMethodSignature*);
using t_mono_type_get_name                  = char* (*)(MonoType*);
using t_mono_signature_get_param_count      = uint32_t (*)(MonoMethodSignature*);
using t_mono_signature_get_params           = MonoType* (*)(MonoMethodSignature*, void**);
using t_mono_method_get_unmanaged_thunk     = void* (*)(MonoMethod*);
using t_mono_string_to_utf8                 = char* (*)(MonoString*);
using t_mono_field_static_get_value         = void (*)(MonoVTable*, MonoClassField*, void*);
using t_mono_field_static_set_value         = void (*)(MonoVTable*, MonoClassField*, void*);
using t_mono_thread_detach                  = void (*)(MonoThread*);
using t_mono_image_get_name                 = const char* (*)(MonoImage*);
using t_mono_lookup_internal_call           = void* (*)(void*);
using t_mono_object_to_string               = MonoString* (*)(MonoObject*);
using t_mono_type_is_void                   = bool (*)(MonoType*);
using t_mono_thread_current                 = MonoThread* (*)();
using t_mono_domain_get_id                  = int32_t (*)(MonoDomain*);
using t_mono_class_get_method_from_name     = MonoMethod* (*)(MonoClass*, const char*, int);
using t_mono_domain_get_friendly_name       = const char* (*)(MonoDomain*);
using t_mono_get_corlib                     = MonoImage* (*)();
using t_mono_image_get_assembly             = MonoAssembly* (*)(MonoImage*);
using t_mono_string_new_wrapper             = MonoString* (*)(const char*);
using t_mono_field_static_get_value         = void (*)(MonoVTable*, MonoClassField*, void*);
using t_mono_value_box                      = MonoObject* (*)(MonoDomain* domain, MonoClass* klass, void* val);

enum MonoTypeEnum : uint8_t {
	MONO_TYPE_END        = 0x00,       /* End of List */
	MONO_TYPE_VOID       = 0x01,
	MONO_TYPE_BOOLEAN    = 0x02,
	MONO_TYPE_CHAR       = 0x03,
	MONO_TYPE_I1         = 0x04,
	MONO_TYPE_U1         = 0x05,
	MONO_TYPE_I2         = 0x06,
	MONO_TYPE_U2         = 0x07,
	MONO_TYPE_I4         = 0x08,
	MONO_TYPE_U4         = 0x09,
	MONO_TYPE_I8         = 0x0a,
	MONO_TYPE_U8         = 0x0b,
	MONO_TYPE_R4         = 0x0c,
	MONO_TYPE_R8         = 0x0d,
	MONO_TYPE_STRING     = 0x0e,
	MONO_TYPE_PTR        = 0x0f,       /* arg: <type> token */
	MONO_TYPE_BYREF      = 0x10,       /* arg: <type> token */
	MONO_TYPE_VALUETYPE  = 0x11,       /* arg: <type> token */
	MONO_TYPE_CLASS      = 0x12,       /* arg: <type> token */
	MONO_TYPE_VAR	     = 0x13,	   /* number */
	MONO_TYPE_ARRAY      = 0x14,       /* type, rank, boundsCount, bound1, loCount, lo1 */
	MONO_TYPE_GENERICINST= 0x15,	   /* <type> <type-arg-count> <type-1> \x{2026} <type-n> */
	MONO_TYPE_TYPEDBYREF = 0x16,
	MONO_TYPE_I          = 0x18,
	MONO_TYPE_U          = 0x19,
	MONO_TYPE_FNPTR      = 0x1b,	      /* arg: full method signature */
	MONO_TYPE_OBJECT     = 0x1c,
	MONO_TYPE_SZARRAY    = 0x1d,       /* 0-based one-dim-array */
	MONO_TYPE_MVAR	     = 0x1e,       /* number */
	MONO_TYPE_CMOD_REQD  = 0x1f,       /* arg: typedef or typeref token */
	MONO_TYPE_CMOD_OPT   = 0x20,       /* optional arg: typedef or typref token */
	MONO_TYPE_INTERNAL   = 0x21,       /* CLR internal type */

	MONO_TYPE_MODIFIER   = 0x40,       /* Or with the following types */
	MONO_TYPE_SENTINEL   = 0x41,       /* Sentinel for varargs method signature */
	MONO_TYPE_PINNED     = 0x45,       /* Local var that points to pinned object */

	MONO_TYPE_ENUM       = 0x55        /* an enumeration */
};

struct MonoVTable
{
    MonoClass* klass;
    /*...*/
};

struct MonoAssemblyName
{
    const char* Name;
    const char* Culture;
    const char* HashValue;
    /*...*/
};

struct MonoGenericClass
{
    MonoClass* klass;
    /*...*/
};

struct MonoType {
    union {
        MonoClass* klass; /* for VALUETYPE and CLASS */
        MonoType* type;   /* for PTR */
        MonoArrayType* array; /* for ARRAY */
        MonoMethodSignature* method;
        MonoGenericParam* generic_param; /* for VAR and MVAR */
        MonoGenericClass* generic_class; /* for GENERICINST */
    } data;
    uint16_t attrs; /* param attributes or field flags */
    MonoTypeEnum type;
    /*...*/
};

struct MonoAssembly
{
    int32_t RefCount;
    char* BaseDir;
    MonoAssemblyName AName;
    // MonoImage* image; I dont provide image as its offset is broken since MonoAssemblyName is not described fully
};

struct MonoReflectionType : MonoObject
{
    MonoType* Type;
};

struct MonoString : MonoObject
{
    int32_t length;
    wchar_t chars[1];
};

#include "mono/mono_headers_def.h" // Other structures with flexible offsets

static t_mono_get_root_domain               mono_get_root_domain;
static t_mono_domain_assembly_open          mono_domain_assembly_open;
static t_mono_thread_attach                 mono_thread_attach;
static t_mono_domain_get                    mono_domain_get;
static t_mono_string_new                    mono_string_new;
static t_mono_assembly_get_image            mono_assembly_get_image;
static t_mono_assembly_foreach              mono_assembly_foreach;
static t_mono_class_from_name               mono_class_from_name;
static t_mono_class_get_methods             mono_class_get_methods;
static t_mono_class_get_field_from_name     mono_class_get_field_from_name;
static t_mono_class_get_namespace           mono_class_get_namespace;
static t_mono_class_get_name                mono_class_get_name;
static t_mono_class_vtable                  mono_class_vtable;
static t_mono_object_get_class              mono_object_get_class;
static t_mono_class_get_type                mono_class_get_type;
static t_mono_method_signature              mono_method_signature;
static t_mono_method_get_name               mono_method_get_name;
static t_mono_compile_method                mono_compile_method;
static t_mono_jit_info_table_find           mono_jit_info_table_find;
static t_mono_jit_info_get_method           mono_jit_info_get_method;
static t_mono_runtime_invoke                mono_runtime_invoke;
static t_mono_object_unbox                  mono_object_unbox;
static t_mono_vtable_get_static_field_data  mono_vtable_get_static_field_data;
static t_mono_type_get_object               mono_type_get_object;
static t_mono_object_new                    mono_object_new;
static t_mono_runtime_object_init           mono_runtime_object_init;
static t_mono_signature_get_return_type     mono_signature_get_return_type;
static t_mono_type_get_name                 mono_type_get_name;
static t_mono_signature_get_param_count     mono_signature_get_param_count;
static t_mono_signature_get_params          mono_signature_get_params;
static t_mono_method_get_unmanaged_thunk    mono_method_get_unmanaged_thunk;
static t_mono_string_to_utf8                mono_string_to_utf8;
static t_mono_field_static_get_value        mono_field_static_get_value;
static t_mono_field_static_set_value        mono_field_static_set_value;
static t_mono_thread_detach                 mono_thread_detach;
static t_mono_image_get_name                mono_image_get_name;
static t_mono_lookup_internal_call          mono_lookup_internal_call;
static t_mono_object_to_string              mono_object_to_string;
static t_mono_type_is_void                  mono_type_is_void;
static t_mono_thread_current                mono_thread_current;
static t_mono_domain_get_id                 mono_domain_get_id;
static t_mono_class_get_method_from_name    mono_class_get_method_from_name;
static t_mono_domain_get_friendly_name      mono_domain_get_friendly_name;
static t_mono_get_corlib                    mono_get_corlib;
static t_mono_image_get_assembly            mono_image_get_assembly;
static t_mono_string_new_wrapper            mono_string_new_wrapper;
static t_mono_value_box                     mono_value_box;

static std::unordered_map<MonoReflectionType*, MonoClass*> g_TypeToClass;

static void _cdecl AssemblyEnumerator(void* assembly, void* params) 
{
    MonoAssembly* monoAssembly = (MonoAssembly*)assembly;
    auto converted = (std::pair<const char*, MonoAssembly**>*)params;
    MonoImage* monoImage = mono_assembly_get_image(monoAssembly);
    const char* assemblyName = mono_image_get_name(monoImage);
    if (strcmp(assemblyName, converted->first) == 0)
        *converted->second = monoAssembly;
}

namespace Mono
{
    void Initialize()
    {
        HMODULE handle = (HMODULE)HaxSdk::GetGlobals().m_BackendHandle;
        GET_BACKEND_API(mono_assembly_foreach);
        GET_BACKEND_API(mono_assembly_get_image);
        GET_BACKEND_API(mono_class_from_name);
        GET_BACKEND_API(mono_class_get_field_from_name);
        GET_BACKEND_API(mono_class_get_methods);
        GET_BACKEND_API(mono_class_get_name);
        GET_BACKEND_API(mono_class_get_namespace);
        GET_BACKEND_API(mono_class_get_type);
        GET_BACKEND_API(mono_class_vtable);
        GET_BACKEND_API(mono_compile_method);
        GET_BACKEND_API(mono_domain_assembly_open);
        GET_BACKEND_API(mono_domain_get);
        GET_BACKEND_API(mono_field_static_get_value);
        GET_BACKEND_API(mono_field_static_set_value);
        GET_BACKEND_API(mono_get_root_domain);
        GET_BACKEND_API(mono_image_get_name);
        GET_BACKEND_API(mono_jit_info_get_method);
        GET_BACKEND_API(mono_jit_info_table_find);
        GET_BACKEND_API(mono_lookup_internal_call);
        GET_BACKEND_API(mono_method_get_name);
        GET_BACKEND_API(mono_method_signature);
        GET_BACKEND_API(mono_object_get_class);
        GET_BACKEND_API(mono_object_new);
        GET_BACKEND_API(mono_object_unbox);
        GET_BACKEND_API(mono_runtime_invoke);
        GET_BACKEND_API(mono_runtime_object_init);
        GET_BACKEND_API(mono_signature_get_param_count);
        GET_BACKEND_API(mono_signature_get_params);
        GET_BACKEND_API(mono_signature_get_return_type);
        GET_BACKEND_API(mono_string_new);
        GET_BACKEND_API(mono_string_to_utf8);
        GET_BACKEND_API(mono_thread_attach);
        GET_BACKEND_API(mono_thread_detach);
        GET_BACKEND_API(mono_type_get_name);
        GET_BACKEND_API(mono_type_get_object);
        GET_BACKEND_API(mono_vtable_get_static_field_data);
        GET_BACKEND_API(mono_thread_current);
        GET_BACKEND_API_OPT(mono_method_get_unmanaged_thunk);
        GET_BACKEND_API_OPT(mono_object_to_string);
        GET_BACKEND_API_OPT(mono_type_is_void);
        GET_BACKEND_API_OPT(mono_domain_get_friendly_name);
        GET_BACKEND_API(mono_domain_get_id);
        GET_BACKEND_API(mono_class_get_method_from_name);
        GET_BACKEND_API(mono_get_corlib);
        GET_BACKEND_API(mono_image_get_assembly);
        GET_BACKEND_API(mono_string_new_wrapper);
        GET_BACKEND_API(mono_value_box);
    }

    void AttachThread()
    {
        mono_thread_attach(mono_get_root_domain());
    }

    MonoDomain* GetDomain()
    {
        return mono_get_root_domain();
    }

    MonoImage* GetCoreLib()
    {
        return mono_get_corlib();
    }

    MonoImage* GetUnityCoreLib()
    {
        MonoDomain* domain = mono_get_root_domain();
        MonoImage* image = LoadAssembly(domain, "UnityEngine.CoreModule");
        if (!image)
        {
            image = LoadAssembly(domain, "UnityEngine");
            HAX_ASSERT(image, "Unable to get unity core assembly");
        }
        return image;
    }

    MonoImage* LoadAssembly(MonoDomain* domain, const char* name)
    {
        MonoAssembly* assembly = nullptr;
        auto params = std::make_pair(name, &assembly);
        mono_assembly_foreach(AssemblyEnumerator, &params);
        return assembly ? mono_assembly_get_image(assembly) : nullptr;
    }

    MonoAssemblyName* AssemblyGetName(MonoImage* image)
    {
        return &mono_image_get_assembly(image)->AName;
    }

    const char* AssemblyNameGetName(MonoAssemblyName* assemblyName)
    {
        return assemblyName->Name;
    }

    MonoReflectionType* AssemblyGetType(MonoImage* image, const char* nameSpace, const char* name)
    {
        MonoClass* klass = mono_class_from_name(image, nameSpace, name);
        if (klass)
        {
            MonoReflectionType* type = mono_type_get_object(mono_get_root_domain(), mono_class_get_type(klass));
            g_TypeToClass[type] = klass;
            return type;
        }
        return nullptr;
    }

    MonoClassField* GetField(MonoReflectionType* type, const char* name)
    {
        return mono_class_get_field_from_name(type->Type->data.klass, name);
    }

    static void GetMethodSignature(MonoMethod* method, char* buff, size_t buffSize)
    {
        MonoMethodSignature* signature = mono_method_signature(method);
        MonoType* returnType = mono_signature_get_return_type(signature);
        strcpy_s(buff, buffSize, mono_type_get_name(returnType));
        strcat_s(buff, buffSize, "(");
        uint32_t nParams = mono_signature_get_param_count(signature);
        if (nParams > 0)
        {
            void* iter = nullptr;
            strcat_s(buff, buffSize, mono_type_get_name(mono_signature_get_params(signature, &iter)));
            while (MonoType* paramType = mono_signature_get_params(signature, &iter))
            {
                strcat_s(buff, buffSize, ",");
                strcat_s(buff, buffSize, mono_type_get_name(paramType));
            }
        }
        strcat_s(buff, buffSize, ")");
    }

    MonoMethod* GetMethod(MonoReflectionType* type, const char* name, const char* sig)
    {
        void* iter = nullptr;
        MonoClass* klass = g_TypeToClass[type];
        HAX_ASSERT(klass, "Type-to-class cache is corrupted!");

        while (MonoMethod* method = mono_class_get_methods(klass, &iter))
        {
            const char* methodName = mono_method_get_name(method);
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

    void GetStaticValue(MonoClassField* field, void* value)
    {
        MonoVTable* vtable = mono_class_vtable(mono_get_root_domain(), field->parent);
        mono_field_static_get_value(vtable, field, value);
    }

    MonoObject* Invoke(MonoMethod* method, void* __this, void** args, MonoException** ex)
    {
        return mono_runtime_invoke((MonoMethod*)method, __this, args, ex);
    }

    wchar_t* StringGetChars(MonoString* str)
    {
        return str->chars;
    }

    int32_t StringGetLength(MonoString* str)
    {
        return str->length;
    }

    const char* ExceptionGetMessage(MonoException* ex)
    {
        return HaxSdk::ToUTF8(ex->message->chars, ex->message->length);
    }

    const char* ExceptionGetStacktrace(MonoException* ex)
    {
        return ex->stack_trace ? HaxSdk::ToUTF8(ex->stack_trace->chars, ex->stack_trace->length) : nullptr;
    }

    int32_t GetFieldOffset(MonoClassField* field)
    {
        return field->offset;
    }

    void* GetStaticFieldAddress(MonoClassField* field)
    {
        if ((field->type->attrs & FIELD_ATTRIBUTE_STATIC) && field->offset >= 0)
        {
            MonoDomain* rootDomain = mono_get_root_domain();
            MonoVTable* vtable = mono_class_vtable(rootDomain, field->parent);
            return (char*)mono_vtable_get_static_field_data(vtable) + field->offset;
        }
        return nullptr;
    }

    void* GetFunctionPointer(MonoMethod* method)
    {
        return mono_compile_method(method);
    }

    void* Unbox(MonoObject* obj)
    {
        return mono_object_unbox(obj);
    }

    void* GetUnmanagedThunk(MonoMethod* method)
    {
        return mono_method_get_unmanaged_thunk ? mono_method_get_unmanaged_thunk(method) : nullptr;
    }

    MonoReflectionType* ObjectGetType(MonoObject* obj)
    {
        MonoClass* klass = obj->Vtable->klass;
        MonoReflectionType* type = mono_type_get_object(mono_get_root_domain(), mono_class_get_type(klass));
        g_TypeToClass[type] = klass;
        return type;
    }

    MonoObject* NewObject(MonoReflectionType* type)
    {
        HAX_ASSERT(type, "NewObject type is null");
        MonoClass* klass = g_TypeToClass[type];
        HAX_ASSERT(klass, "Type-to-class cache is corrupted!");
        MonoObject* obj = mono_object_new(mono_get_root_domain(), klass);
        mono_runtime_object_init(obj);
        return obj;
    }

    MonoObject* AllocObject(MonoReflectionType* type)
    {
        HAX_ASSERT(type, "NewObject type is null");
        MonoClass* klass = g_TypeToClass[type];
        HAX_ASSERT(type, "Type-to-class cache is corrupted!");
        return mono_object_new(mono_get_root_domain(), klass);
    }

    MonoString* NewString(const char* text)
    {
        return mono_string_new_wrapper(text);
    }

    MonoObject* BoxValue(MonoReflectionType* type, void* value)
    {
       return mono_value_box(mono_get_root_domain(), g_TypeToClass[type], value);
    }
}