#include "haxsdk_il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>

#include "../../haxsdk_logger.h"
#include "../../haxsdk_assertion.h"
#include "../haxsdk_backend.h"

#define GET_BACKEND_API(n)                          \
    n = (t ## _ ## n)(GetProcAddress(handle, #n));  \
    HAX_LOG_DEBUG("{} {}", #n, (void*)n);           \
    HAX_ASSERT(n)

#define GET_BACKEND_API_OPT(n)                      \
    n = (t ## _ ## n)(GetProcAddress(handle, #n));  \
    HAX_LOG_DEBUG("{} {}", #n, (void*)n)

#define FIELD_ATTRIBUTE_STATIC 0x0010

using t_il2cpp_assembly_get_image           = Il2CppImage* (*)(Il2CppAssembly* assembly);
using t_il2cpp_class_from_name              = Il2CppClass* (*)(Il2CppImage* image, const char* namespaze, const char* name);
using t_il2cpp_class_from_system_type       = Il2CppClass* (*)(Il2CppReflectionType*);
using t_il2cpp_class_get_field_from_name    = Il2CppField* (*)(Il2CppClass* pClass, const char* name);
using t_il2cpp_class_get_methods            = Il2CppMethod * (*)(Il2CppClass* pClass, void** ppIter);
using t_il2cpp_class_get_static_field_data  = void* (*)(Il2CppClass*);
using t_il2cpp_class_get_type               = Il2CppType* (*)(Il2CppClass* pClass);
using t_il2cpp_domain_assembly_open         = Il2CppAssembly* (*)(Il2CppDomain* domain, const char* name);
using t_il2cpp_domain_get                   = Il2CppDomain* (*)();
using t_il2cpp_domain_get_assemblies        = Il2CppAssembly** (*)(Il2CppDomain* domain, size_t* size);
using t_il2cpp_field_static_get_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_field_static_set_value       = void (*)(Il2CppField* pField, void* pValue);
using t_il2cpp_get_corlib                   = Il2CppImage* (*)();
using t_il2cpp_image_get_assembly           = Il2CppAssembly* (*)(Il2CppImage*);
using t_il2cpp_image_get_name               = const char* (*)(Il2CppImage*);
using t_il2cpp_method_get_class             = Il2CppClass* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_param             = Il2CppType* (*)(Il2CppMethod* pMethod, uint32_t index);
using t_il2cpp_method_get_param_count       = int32_t(*)(Il2CppMethod* pMethod);
using t_il2cpp_method_get_return_type       = Il2CppType* (*)(Il2CppMethod* pMethod);
using t_il2cpp_object_new                   = Il2CppObject* (*)(Il2CppClass* pClass);
using t_il2cpp_object_unbox                 = void* (*)(Il2CppObject* obj);
using t_il2cpp_runtime_invoke               = Il2CppObject* (*)(Il2CppMethod* pMethod, void* obj, void** params, Il2CppException** exc);
using t_il2cpp_runtime_object_init          = void (*)(Il2CppObject* obj);
using t_il2cpp_string_new                   = Il2CppString* (*)(const char* str);
using t_il2cpp_thread_attach                = Il2CppThread* (*)(Il2CppDomain* domain);
using t_il2cpp_thread_detach                = void (*)(Il2CppThread*);
using t_il2cpp_type_get_name                = const char* (*)(Il2CppType* type);
using t_il2cpp_type_get_object              = Il2CppReflectionType* (*)(Il2CppType* type);
using t_il2cpp_value_box                    = Il2CppObject* (*)(Il2CppClass*, void*);
using t_il2cpp_gchandle_new                 = uint32_t (*)(Il2CppObject*, bool);
using t_il2cpp_gchandle_new_weakref         = uint32_t (*)(Il2CppObject*, bool);
using t_il2cpp_gchandle_get_target          = Il2CppObject* (*)(uint32_t);
using t_il2cpp_gchandle_free                = void (*)(uint32_t);

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

struct Il2CppField
{
    const char* name;
    const Il2CppType* type;
    Il2CppClass* parent;
    int32_t offset; // If offset is -1, then it's thread static
    uint32_t token;
};

struct Il2CppMethod
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
static t_il2cpp_gchandle_new                il2cpp_gchandle_new;
static t_il2cpp_gchandle_new_weakref        il2cpp_gchandle_new_weakref;
static t_il2cpp_gchandle_get_target         il2cpp_gchandle_get_target;
static t_il2cpp_gchandle_free               il2cpp_gchandle_free;

static void GetMethodSignature(Il2CppMethod* method, char* buff, size_t buffSize);

namespace il2cpp
{
    void Initialize()
    {
        HMODULE handle = static_cast<HMODULE>(HaxSdk::GetBackendHandle());
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
        GET_BACKEND_API(il2cpp_gchandle_new);
        GET_BACKEND_API(il2cpp_gchandle_new_weakref);
        GET_BACKEND_API(il2cpp_gchandle_get_target);
        GET_BACKEND_API(il2cpp_gchandle_free);
    }

    Il2CppImage* Assembly::GetImage(Il2CppAssembly* assembly)
    {
        HAX_ASSERT(assembly);
        return il2cpp_assembly_get_image(assembly);
    }

    Il2CppAssembly* Assembly::Load(Il2CppDomain* domain, const char* name)
    {
        HAX_ASSERT(domain);
        return il2cpp_domain_assembly_open(domain, name);
    }

    Il2CppClass* Class::FromName(Il2CppImage* image, const char* nameSpace, const char* name)
    {
        HAX_ASSERT(image);
        return il2cpp_class_from_name(image, nameSpace, name);
    }

    Il2CppClass* Class::FromSystemType(Il2CppReflectionType* type)
    {
        HAX_ASSERT(type);
        return il2cpp_class_from_system_type(type);
    }

    Il2CppField* Class::GetFieldFromName(Il2CppClass* klass, const char* name)
    {
        HAX_ASSERT(klass);
        return il2cpp_class_get_field_from_name(klass, name);
    }

    Il2CppMethod* Class::GetMethodFromName(Il2CppClass* klass, const char* name, const char* signature)
    {
        void* iter = nullptr;
        while (Il2CppMethod* method = il2cpp_class_get_methods(klass, &iter))
        {
            const char* methodName = il2cpp_method_get_name(method);
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

    void* Class::GetStaticFieldData(Il2CppClass* klass)
    {
        HAX_ASSERT(klass);
        return il2cpp_class_get_static_field_data(klass);
    }

    Il2CppType* Class::GetType(Il2CppClass* klass)
    {
        HAX_ASSERT(klass);
        return il2cpp_class_get_type(klass);
    }

    Il2CppDomain* Domain::GetCurrent()
    {
        return il2cpp_domain_get();
    }

    Il2CppString* Exception::GetMessage(Il2CppException* ex)
    {
        HAX_ASSERT(ex);
        return ex->message;
    }

    Il2CppException* Exception::GetNullReferenceException()
    {
        Il2CppImage* image = il2cpp_get_corlib();
        Il2CppClass* klass = il2cpp_class_from_name(image, "System", "NullReferenceException");
        Il2CppObject* ex = Object::New(klass);
        Runtime::ObjectInit(ex);
        return (Il2CppException*)ex;
    }

    Il2CppException* Exception::GetArgumentOutOfRangeException()
    {
        Il2CppImage* image = il2cpp_get_corlib();
        Il2CppClass* klass = il2cpp_class_from_name(image, "System", "ArgumentOutOfRangeException");
        Il2CppObject* ex = Object::New(klass);
        Runtime::ObjectInit(ex);
        return (Il2CppException*)ex;
    }

    bool Field::IsStatic(Il2CppField* field)
    {
        HAX_ASSERT(field);
        return field->type->attrs & FIELD_ATTRIBUTE_STATIC;
    }

    size_t Field::GetOffset(Il2CppField* field)
    {
        HAX_ASSERT(field);
        return static_cast<size_t>(field->offset);
    }

    Il2CppClass* Field::GetParent(Il2CppField* field)
    {
        HAX_ASSERT(field);
        return field->parent;
    }

    void* Field::GetAddress(Il2CppField* field, Il2CppObject* __this)
    {
        if (IsStatic(field))
            return (char*)Class::GetStaticFieldData(field->parent) + field->offset;

        HAX_ASSERT_E(__this, "THIS not provided for non-static field");
        return (char*)__this + field->offset;
    }

    void Field::StaticGetValue(Il2CppField* field, void* value)
    {
        HAX_ASSERT(field);
        return il2cpp_field_static_get_value(field, value);
    }

    void Field::StaticSetValue(Il2CppField* field, void* value)
    {
        HAX_ASSERT(field);
        il2cpp_field_static_set_value(field, value);
    }

    uint32_t GCHandle::New(Il2CppObject * obj, bool pinned)
    {
        HAX_ASSERT(obj);
        return il2cpp_gchandle_new(obj, pinned);
    }

    uint32_t GCHandle::NewWeakRef(Il2CppObject* obj, bool trackResurrection)
    {
        HAX_ASSERT(obj);
        return il2cpp_gchandle_new_weakref(obj, trackResurrection);
    }

    Il2CppObject* GCHandle::GetTaget(uint32_t handle)
    {
        HAX_ASSERT(handle);
        return il2cpp_gchandle_get_target(handle);
    }

    void GCHandle::Free(uint32_t handle)
    {
        HAX_ASSERT(handle);
        il2cpp_gchandle_free(handle);
    }

    Il2CppClass* ReflectionType::GetClass(Il2CppReflectionType* type)
    {
        return il2cpp_class_from_system_type(type);
    }

    Il2CppImage* Image::GetCorlib()
    {
        return il2cpp_get_corlib();
    }

    Il2CppAssembly* Image::GetAssembly(Il2CppImage* image)
    {
        HAX_ASSERT(image);
        return il2cpp_image_get_assembly(image);
    }

    const char* Image::GetName(Il2CppImage* image)
    {
        HAX_ASSERT(image);
        return il2cpp_image_get_name(image);
    }

    Il2CppClass* Method::GetClass(Il2CppMethod* method)
    {
        HAX_ASSERT(method);
        return il2cpp_method_get_class(method);
    }

    const char* Method::GetName(Il2CppMethod* method)
    {
        HAX_ASSERT(method);
        return il2cpp_method_get_name(method);
    }

    Il2CppType* Method::GetParam(Il2CppMethod* method, uint32_t index)
    {
        HAX_ASSERT(method);
        return il2cpp_method_get_param(method, index);
    }

    int32_t Method::GetParamCount(Il2CppMethod* method)
    {
        HAX_ASSERT(method);
        return il2cpp_method_get_param_count(method);
    }

    Il2CppType* Method::GetReturnType(Il2CppMethod* method)
    {
        HAX_ASSERT(method);
        return il2cpp_method_get_return_type(method);
    }

    Il2CppObject* Object::Box(Il2CppClass* klass, void* data)
    {
        HAX_ASSERT(klass && data);
        return il2cpp_value_box(klass, data);
    }

    Il2CppObject* Object::New(Il2CppClass* klass)
    {
        HAX_ASSERT(klass);
        return il2cpp_object_new(klass);
    }

    Il2CppClass* Object::GetClass(Il2CppObject* object)
    {
        HAX_ASSERT(object);
        return object->Klass;
    }

    void* Object::Unbox(Il2CppObject* object)
    {
        HAX_ASSERT(object);
        return il2cpp_object_unbox(object);
    }

    Il2CppReflectionType* Reflection::TypeGetObject(Il2CppType* type)
    {
        HAX_ASSERT(type);
        return il2cpp_type_get_object(type);
    }

    Il2CppObject* Runtime::Invoke(Il2CppMethod* method, void* __this, void** args, Il2CppException** ex)
    {
        HAX_ASSERT(method);
        return il2cpp_runtime_invoke(method, __this, args, ex);
    }
    
    void Runtime::ObjectInit(Il2CppObject* object)
    {
        HAX_ASSERT(object);
        return il2cpp_runtime_object_init(object);
    }

    Il2CppString* String::New(const char* str)
    {
        return il2cpp_string_new(str);
    }

    Il2CppThread* Thread::Attach(Il2CppDomain* domain)
    {
        HAX_ASSERT(domain);
        return il2cpp_thread_attach(domain);
    }

    void Thread::Detach(Il2CppThread* thread)
    {
        HAX_ASSERT(thread);
        return il2cpp_thread_detach(thread);
    }

    const char* Type::GetName(Il2CppType* type)
    {
        HAX_ASSERT(type);
        return il2cpp_type_get_name(type);
    }
}

static void GetMethodSignature(Il2CppMethod* method, char* buff, size_t buffSize)
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