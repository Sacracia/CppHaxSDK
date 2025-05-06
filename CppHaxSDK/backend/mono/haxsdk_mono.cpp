#include "haxsdk_mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>

#include "../haxsdk_backend.h"
#include "../../haxsdk_logger.h"
#include "../../haxsdk_assertion.h"

#define GET_BACKEND_API(n)                          \
    n = (t ## _ ## n)(GetProcAddress(handle, #n));  \
    HAX_LOG_DEBUG("{} {}", #n, (void*)n);           \
    HAX_ASSERT(n)

#define GET_BACKEND_API_OPT(n)                      \
    n = (t ## _ ## n)(GetProcAddress(handle, #n));  \
    HAX_LOG_DEBUG("{} {}", #n, (void*)n)

#define FIELD_ATTRIBUTE_STATIC 0x0010

using GFunc                                 = void(__cdecl*)(void* data, void* user_data);
using t_mono_assembly_foreach               = void (*)(GFunc, void*);
using t_mono_assembly_get_image             = MonoImage* (*)(MonoAssembly*);
using t_mono_class_from_name                = MonoClass* (*)(MonoImage*, const char*, const char*);
using t_mono_class_get_field_from_name      = MonoClassField* (*)(MonoClass*, const char*);
using t_mono_class_get_method_from_name     = MonoMethod* (*)(MonoClass*, const char*, int);
using t_mono_class_get_methods              = MonoMethod* (*)(MonoClass*, void**);
using t_mono_class_get_name                 = const char* (*)(MonoClass*);
using t_mono_class_get_namespace            = const char* (*)(MonoClass*);
using t_mono_class_get_type                 = MonoType* (*)(MonoClass*);
using t_mono_class_vtable                   = MonoVTable* (*)(MonoDomain*, MonoClass*);
using t_mono_compile_method                 = void* (*)(MonoMethod*);
using t_mono_domain_assembly_open           = MonoAssembly* (*)(MonoDomain*, const char*);
using t_mono_domain_get                     = MonoDomain* (*)();
using t_mono_domain_get_friendly_name       = const char* (*)(MonoDomain*);
using t_mono_domain_get_id                  = int32_t (*)(MonoDomain*);
using t_mono_field_static_get_value         = void (*)(MonoVTable*, MonoClassField*, void*);
//using t_mono_field_static_get_value         = void (*)(MonoVTable*, MonoClassField*, void*);
using t_mono_field_static_set_value         = void (*)(MonoVTable*, MonoClassField*, void*);
using t_mono_get_corlib                     = MonoImage* (*)();
using t_mono_get_root_domain                = MonoDomain* (*)();
using t_mono_image_get_assembly             = MonoAssembly* (*)(MonoImage*);
using t_mono_image_get_name                 = const char* (*)(MonoImage*);
//using t_mono_jit_info_get_method            = MonoMethod* (*)(void*);
//using t_mono_jit_info_table_find            = void* (*)(MonoDomain*, void*);
//using t_mono_lookup_internal_call           = void* (*)(void*);
using t_mono_method_get_name                = const char* (*)(MonoMethod*);
using t_mono_method_get_unmanaged_thunk     = void* (*)(MonoMethod*);
using t_mono_method_signature               = MonoMethodSignature* (*)(MonoMethod*);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
//using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
using t_mono_object_new                     = MonoObject * (*)(MonoDomain*, MonoClass*);
using t_mono_object_to_string               = MonoString* (*)(MonoObject*);
using t_mono_object_unbox                   = void* (*)(MonoObject*);
using t_mono_print_unhandled_exception      = void (*)(MonoObject*);
using t_mono_runtime_invoke                 = MonoObject* (*)(MonoMethod*, void*, void**, MonoException**);
using t_mono_runtime_object_init            = void (*)(MonoObject*);
using t_mono_signature_get_param_count      = uint32_t (*)(MonoMethodSignature*);
using t_mono_signature_get_params           = MonoType* (*)(MonoMethodSignature*, void**);
using t_mono_signature_get_return_type      = MonoType* (*)(MonoMethodSignature*);
using t_mono_string_new                     = MonoString* (*)(MonoDomain*, const char*);
//using t_mono_string_new_wrapper             = MonoString* (*)(const char*);
using t_mono_string_to_utf8                 = char* (*)(MonoString*);
using t_mono_thread_attach                  = MonoThread* (*)(MonoDomain*);
using t_mono_thread_current                 = MonoThread* (*)();
using t_mono_thread_detach                  = void (*)(MonoThread*);
using t_mono_type_get_name                  = char* (*)(MonoType*);
using t_mono_type_get_object                = MonoReflectionType* (*)(MonoDomain*, MonoType*);
using t_mono_type_is_void                   = bool (*)(MonoType*);
using t_mono_value_box                      = MonoObject* (*)(MonoDomain* domain, MonoClass* klass, void* val);
using t_mono_vtable_get_static_field_data   = void* (*)(MonoVTable*);
using t_mono_gchandle_new                   = uint32_t (*)(MonoObject*, bool);
using t_mono_gchandle_new_weakref           = uint32_t (*)(MonoObject*, bool);
using t_mono_gchandle_get_target            = MonoObject* (*)(uint32_t);
using t_mono_gchandle_free                  = void (*)(uint32_t);


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

struct MonoObject
{
    MonoVTable* Vtable;
    MonoThreadsSync* Synchronisation;
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

#if defined(UNITY_5_5)
#include "headers/unity-5.5.h"
#elif defined(UNITY_5_6)
#include "headers/unity-5.6.h"
#elif defined(UNITY_2017_1)
#include "headers/unity-2017.1.h"
#elif defined(UNITY_2017_2)
#include "headers/unity-2017.2.h"
#elif defined(UNITY_2017_3)
#include "headers/unity-2017.3.h"
#elif defined(UNITY_2017_4)
#include "headers/unity-2017.4.h"
#elif defined(UNITY_2018_1)
#include "headers/unity-2018.1.h"
#elif defined(UNITY_2018_2)
#include "headers/unity-2018.2.h"
#elif defined(UNITY_2018_3)
#include "headers/unity-2018.3.h"
#elif defined(UNITY_2018_4)
#include "headers/unity-2018.4.h"
#elif defined(UNITY_2022_1)
#include "headers/unity-2022.1-mbe.h"
#elif defined(UNITY_2022_2)
#include "headers/unity-2022.2-mbe.h"
#elif defined(UNITY_2022_3)
#include "headers/unity-2022.3-mbe.h"
#endif

static void GetMethodSignature(MonoMethod* method, char* buff, size_t buffSize);
static void _cdecl AssemblyEnumerator(void* assembly, void* params);

static t_mono_assembly_foreach              mono_assembly_foreach;
static t_mono_assembly_get_image            mono_assembly_get_image;
static t_mono_class_from_name               mono_class_from_name;
static t_mono_class_get_field_from_name     mono_class_get_field_from_name;
static t_mono_class_get_method_from_name    mono_class_get_method_from_name;
static t_mono_class_get_methods             mono_class_get_methods;
static t_mono_class_get_name                mono_class_get_name;
static t_mono_class_get_namespace           mono_class_get_namespace;
static t_mono_class_get_type                mono_class_get_type;
static t_mono_class_vtable                  mono_class_vtable;
static t_mono_compile_method                mono_compile_method;
static t_mono_domain_assembly_open          mono_domain_assembly_open;
static t_mono_domain_get                    mono_domain_get;
static t_mono_domain_get_friendly_name      mono_domain_get_friendly_name;
static t_mono_domain_get_id                 mono_domain_get_id;
static t_mono_field_static_get_value        mono_field_static_get_value;
static t_mono_field_static_set_value        mono_field_static_set_value;
static t_mono_get_corlib                    mono_get_corlib;
static t_mono_get_root_domain               mono_get_root_domain;
static t_mono_image_get_assembly            mono_image_get_assembly;
static t_mono_image_get_name                mono_image_get_name;
//static t_mono_jit_info_get_method           mono_jit_info_get_method;
//static t_mono_jit_info_table_find           mono_jit_info_table_find;
//static t_mono_lookup_internal_call          mono_lookup_internal_call;
static t_mono_method_get_name               mono_method_get_name;
static t_mono_method_get_unmanaged_thunk    mono_method_get_unmanaged_thunk;
static t_mono_method_signature              mono_method_signature;
static t_mono_object_get_class              mono_object_get_class;
static t_mono_object_new                    mono_object_new;
static t_mono_object_to_string              mono_object_to_string;
static t_mono_object_unbox                  mono_object_unbox;
static t_mono_runtime_invoke                mono_runtime_invoke;
static t_mono_runtime_object_init           mono_runtime_object_init;
static t_mono_signature_get_param_count     mono_signature_get_param_count;
static t_mono_signature_get_params          mono_signature_get_params;
static t_mono_signature_get_return_type     mono_signature_get_return_type;
static t_mono_string_new                    mono_string_new;
//static t_mono_string_new_wrapper            mono_string_new_wrapper;
static t_mono_string_to_utf8                mono_string_to_utf8;
static t_mono_thread_attach                 mono_thread_attach;
static t_mono_thread_current                mono_thread_current;
static t_mono_thread_detach                 mono_thread_detach;
static t_mono_type_get_name                 mono_type_get_name;
static t_mono_type_get_object               mono_type_get_object;
static t_mono_type_is_void                  mono_type_is_void;
static t_mono_value_box                     mono_value_box;
static t_mono_vtable_get_static_field_data  mono_vtable_get_static_field_data;
static t_mono_gchandle_new                  mono_gchandle_new;
static t_mono_gchandle_new_weakref          mono_gchandle_new_weakref;
static t_mono_gchandle_get_target           mono_gchandle_get_target;
static t_mono_gchandle_free                 mono_gchandle_free;

namespace mono
{
    void Initialize()
    {
        HMODULE handle = static_cast<HMODULE>(HaxSdk::GetBackendHandle());
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
        //GET_BACKEND_API(mono_jit_info_get_method);
        //GET_BACKEND_API(mono_jit_info_table_find);
        //GET_BACKEND_API(mono_lookup_internal_call);
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
        //GET_BACKEND_API(mono_string_new_wrapper);
        GET_BACKEND_API(mono_value_box);
        GET_BACKEND_API(mono_gchandle_new);
        GET_BACKEND_API(mono_gchandle_new_weakref);
        GET_BACKEND_API(mono_gchandle_get_target);
        GET_BACKEND_API(mono_gchandle_free);
    }

    MonoImage* Assembly::GetImage(MonoAssembly* assembly)
    {
        return mono_assembly_get_image(assembly);
    }

    MonoAssembly* Assembly::Load(MonoDomain* domain, const char* name)
    {
        HAX_ASSERT(domain);
        MonoAssembly* assembly = nullptr;
        auto params = std::make_pair(name, &assembly);
        mono_assembly_foreach(AssemblyEnumerator, &params);
        return assembly;
    }

    MonoClass* Class::FromName(MonoImage* image, const char* nameSpace, const char* name)
    {
        HAX_ASSERT(image);
        return mono_class_from_name(image, nameSpace, name);
    }

    MonoClassField* Class::GetFieldFromName(MonoClass* klass, const char* name)
    {
        HAX_ASSERT(klass);
        return mono_class_get_field_from_name(klass, name);
    }

    MonoMethod* Class::GetMethodFromName(MonoClass* klass, const char* name, const char* signature)
    {
        HAX_ASSERT(klass);
        void* iter = nullptr;
        while (MonoMethod* method = mono_class_get_methods(klass, &iter))
        {
            const char* methodName = mono_method_get_name(method);
            if (strcmp(methodName, name) == 0)
            {
                if (!signature || !signature[0])
                    return method;

                char buff[256] = { 0 };
                GetMethodSignature(method, buff, sizeof(buff));
                if (strcmp(buff, signature) == 0)
                    return method;
            }
        }
        return nullptr;
    }

    const char* Class::GetName(MonoClass* klass)
    {
        HAX_ASSERT(klass);
        return mono_class_get_name(klass);
    }

    const char* Class::GetNamespace(MonoClass* klass)
    {
        HAX_ASSERT(klass);
        return mono_class_get_namespace(klass);
    }

    MonoType* Class::GetType(MonoClass* klass)
    {
        HAX_ASSERT(klass);
        return mono_class_get_type(klass);
    }

    MonoVTable* Class::VTable(MonoDomain* domain, MonoClass* klass)
    {
        HAX_ASSERT((klass && domain));
        return mono_class_vtable(domain, klass);
    }

    MonoDomain* Domain::GetCurrent()
    {
        return mono_domain_get();
    }

    MonoDomain* Domain::GetRoot()
    {
        return mono_get_root_domain();
    }

    MonoString* Exception::GetMessage(MonoException* ex)
    {
        HAX_ASSERT(ex);
        return ex->message;
    }

    MonoException* Exception::GetNullReferenceException()
    {
        MonoImage* image = mono_get_corlib();
        MonoClass* klass = mono_class_from_name(image, "System", "NullReferenceException");
        MonoObject* ex = Object::New(mono_get_root_domain(), klass);
        Runtime::ObjectInit(ex);
        return (MonoException*)ex;
    }

    MonoException* Exception::GetArgumentOutOfRangeException()
    {
        MonoImage* image = mono_get_corlib();
        MonoClass* klass = mono_class_from_name(image, "System", "ArgumentOutOfRangeException");
        MonoObject* ex = Object::New(mono_get_root_domain(), klass);
        Runtime::ObjectInit(ex);
        return (MonoException*)ex;
    }

    bool Field::IsStatic(MonoClassField* field)
    {
        HAX_ASSERT(field);
        return field->type->attrs & FIELD_ATTRIBUTE_STATIC;
    }

    MonoClass* Field::GetParent(MonoClassField* field)
    {
        HAX_ASSERT(field);
        return field->parent;
    }

    void* Field::GetAddress(MonoClassField* field, MonoObject* __this)
    {
        if (IsStatic(field))
        {
            MonoVTable* vtable = Class::VTable(Domain::GetRoot(), field->parent);
            return (char*)VTable::GetStaticFieldData(vtable) + field->offset;
        }

        HAX_ASSERT_E(__this, "THIS not provided for non-static field");
        return (char*)__this + field->offset;
    }

    void Field::StaticGetValue(MonoVTable* vtable, MonoClassField* field, void* value)
    {
        HAX_ASSERT(vtable && field);
        mono_field_static_get_value(vtable, field, value);
    }

    void Field::StaticSetValue(MonoVTable* vtable, MonoClassField* field, void* value)
    {
        HAX_ASSERT(vtable && field);
        mono_field_static_set_value(vtable, field, value);
    }

    uint32_t GCHandle::New(MonoObject* obj, bool pinned)
    {
        HAX_ASSERT(obj);
        return mono_gchandle_new(obj, pinned);
    }

    uint32_t GCHandle::NewWeakRef(MonoObject* obj, bool trackResurrection)
    {
        HAX_ASSERT(obj);
        return mono_gchandle_new_weakref(obj, trackResurrection);
    }

    MonoObject* GCHandle::GetTaget(uint32_t handle)
    {
        HAX_ASSERT(handle);
        return mono_gchandle_get_target(handle);
    }

    void GCHandle::Free(uint32_t handle)
    {
        HAX_ASSERT(handle);
        mono_gchandle_free(handle);
    }

    MonoImage* Image::GetCorlib()
    {
        return mono_get_corlib();
    }

    MonoAssembly* Image::GetAssembly(MonoImage* image)
    {
        HAX_ASSERT(image);
        return mono_image_get_assembly(image);
    }

    const char* Image::GetName(MonoImage* image)
    {
        HAX_ASSERT(image);
        return mono_image_get_name(image);
    }

    void* Method::Compile(MonoMethod* method)
    {
        HAX_ASSERT(method);
        return mono_compile_method(method);
    }

    const char* Method::GetName(MonoMethod* method)
    {
        HAX_ASSERT(method);
        return mono_method_get_name(method);
    }

    void* Method::GetUnmanagedThunk(MonoMethod* method)
    {
        HAX_ASSERT(method);
        return mono_method_get_unmanaged_thunk(method);
    }

    MonoMethodSignature* Method::Signature(MonoMethod* method)
    {
        HAX_ASSERT(method);
        return mono_method_signature(method);
    }

    MonoObject* Object::Box(MonoDomain* domain, MonoClass* klass, void* data)
    {
        HAX_ASSERT(klass);
        return mono_value_box(domain, klass, data);
    }

    MonoClass* Object::GetClass(MonoObject* object)
    {
        HAX_ASSERT(object);
        return mono_object_get_class(object);
    }

    MonoObject* Object::New(MonoDomain* domain, MonoClass* klass)
    {
        HAX_ASSERT(domain && klass);
        return mono_object_new(domain, klass);
    }

    void* Object::Unbox(MonoObject* object)
    {
        HAX_ASSERT(object);
        return mono_object_unbox(object);
    }

    MonoClass* ReflectionType::GetClass(MonoReflectionType* type)
    {
        HAX_ASSERT(type);
        return type->Type->data.klass;
    }

    MonoObject* Runtime::Invoke(MonoMethod* method, void* __this, void** args, MonoException** ex)
    {
        HAX_ASSERT(method);
        return mono_runtime_invoke(method, __this, args, ex);
    }
   
    void Runtime::ObjectInit(MonoObject* object)
    {
        HAX_ASSERT(object);
        return mono_runtime_object_init(object);
    }

    uint32_t Signature::GetParamCount(MonoMethodSignature* methodSig)
    {
        HAX_ASSERT(methodSig);
        return mono_signature_get_param_count(methodSig);
    }

    MonoType* Signature::GetParams(MonoMethodSignature* methodSig, void** iter)
    {
        HAX_ASSERT(methodSig);
        return mono_signature_get_params(methodSig, iter);
    }

    MonoType* Signature::GetReturnType(MonoMethodSignature* methodSig)
    {
        HAX_ASSERT(methodSig);
        return mono_signature_get_return_type(methodSig);
    }

    MonoString* String::New(MonoDomain* domain, const char* str)
    {
        HAX_ASSERT(domain);
        return mono_string_new(domain, str);
    }

    MonoThread* Thread::Attach(MonoDomain* domain)
    {
        HAX_ASSERT(domain);
        return mono_thread_attach(domain);
    }

    MonoThread* Thread::Current()
    {
        return mono_thread_current();
    }

    void Thread::Detach(MonoThread* thread)
    {
        HAX_ASSERT(domain);
        return mono_thread_detach(thread);
    }

    char* Type::GetName(MonoType* type)
    {
        HAX_ASSERT(type);
        return mono_type_get_name(type);
    }

    MonoReflectionType* Reflection::TypeGetObject(MonoDomain* domain, MonoType* type)
    {
        HAX_ASSERT(domain && type);
        return mono_type_get_object(domain, type);
    }

    void* VTable::GetStaticFieldData(MonoVTable* vtable)
    {
        HAX_ASSERT(vtable);
        return mono_vtable_get_static_field_data(vtable);
    }
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

static void _cdecl AssemblyEnumerator(void* assembly, void* params)
{
    MonoAssembly* monoAssembly = (MonoAssembly*)assembly;
    auto converted = (std::pair<const char*, MonoAssembly**>*)params;
    MonoImage* monoImage = mono_assembly_get_image(monoAssembly);
    const char* assemblyName = mono_image_get_name(monoImage);
    if (strcmp(assemblyName, converted->first) == 0)
        *converted->second = monoAssembly;
}