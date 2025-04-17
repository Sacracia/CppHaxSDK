#include "haxsdk_mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <format>
#include <iostream>

#include "../haxsdk.h"

#define GET_BACKEND_API(n)      n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n));\
                                HAX_ASSERT(n, #n)
#define GET_BACKEND_API_OPT(n)  n = (t ## _ ## n)(GetProcAddress(handle, #n));\
                                HaxSdk::Log(std::format("{} {}", #n, (void*)n))

static void _cdecl AssemblyEnumerator(void* assembly, void* params);
static void GetMethodSignature(MonoMethod* method, char* buff, size_t buffSize);
static MonoAssembly* FindAssembly(const char* assembly);

void Mono::Initialize()
{
    HMODULE handle = (HMODULE)HaxSdk::GetGlobals().BackendHandle;
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
}

void Mono::AttachThread()
{
    mono_thread_attach(mono_get_root_domain());
}

bool Mono::AssemblyExists(const char* assembly, MonoAssembly** out)
{
    MonoAssembly* monoAssembly = FindAssembly(assembly);
    if (monoAssembly && out)
        *out = monoAssembly;
    return monoAssembly;
}

int32_t Mono::FindField(const char* assembly, const char* nameSpace, const char* klass, const char* field)
{
    MonoAssembly* monoAssembly = FindAssembly(assembly);
    HAX_ASSERT(monoAssembly, std::format("Assembly {} not found", assembly).c_str());

    MonoClass* monoClass = mono_class_from_name(mono_assembly_get_image(monoAssembly), nameSpace, klass);
    HAX_ASSERT(monoClass, std::format("Class {}.{} not found in {}", nameSpace, klass, assembly).c_str());

    MonoField* monoField = mono_class_get_field_from_name(monoClass, field);
    HAX_ASSERT(monoField, std::format("Field {} not found in {}.{}", field,  nameSpace, klass).c_str());

    return monoField->Offset;
}

void* Mono::FindStaticField(const char* assembly, const char* nameSpace, const char* klass, const char* field)
{
    MonoAssembly* monoAssembly = FindAssembly(assembly);
    HAX_ASSERT(monoAssembly, std::format("Assembly {} not found", assembly).c_str());

    MonoClass* monoClass = mono_class_from_name(mono_assembly_get_image(monoAssembly), nameSpace, klass);
    HAX_ASSERT(monoClass, std::format("Class {}.{} not found in {}", nameSpace, klass, assembly).c_str());

    MonoField* monoField = mono_class_get_field_from_name(monoClass, field);
    HAX_ASSERT(monoField, std::format("Field {} not found in {}.{}", field, nameSpace, klass).c_str());

    MonoVTable* vtable = mono_class_vtable(mono_get_root_domain(), monoClass);
    void* data = mono_vtable_get_static_field_data(vtable);
    return (char*)data + monoField->Offset;
}

MethodParams Mono::FindMethod(const char* assembly, const char* nameSpace, const char* klass, const char* method, const char* sig)
{
    MonoAssembly* monoAssembly = FindAssembly(assembly);
    HAX_ASSERT(monoAssembly, std::format("Assembly {} not found", assembly).c_str());

    MonoClass* monoClass = mono_class_from_name(mono_assembly_get_image(monoAssembly), nameSpace, klass);
    HAX_ASSERT(monoClass, std::format("Class {}.{} not found in {}", nameSpace, klass, assembly).c_str());

    void* iter = nullptr;
    MonoMethod* monoMethod = nullptr;
    while (monoMethod = mono_class_get_methods(monoClass, &iter))
    {
        const char* methodName = mono_method_get_name(monoMethod);
        if (strcmp(methodName, method) != 0)
            continue;

        if (!sig || sig[0] == '\0')
            break;

        char buff[256] = {0};
        GetMethodSignature(monoMethod, buff, sizeof(buff));
        if (strcmp(buff, sig) == 0)
            break;
    }

    HAX_ASSERT(monoMethod, std::format("Method {} not found in {}.{}", method, nameSpace, klass).c_str());

    MethodParams params{};
    params.BackendMethod = (void*)monoMethod;
    HaxSdk::Log(std::format("{}", (void*)monoMethod));
    params.Address = mono_compile_method(monoMethod);
    if (mono_method_get_unmanaged_thunk)
        params.Thunk = mono_method_get_unmanaged_thunk(monoMethod);
    return params;
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

static void GetMethodSignature(MonoMethod* method, char* buff, size_t buffSize)
{
    MonoMethodSignature* signature = mono_method_signature(method);
    MonoType* retType = mono_signature_get_return_type(signature);
    strcpy_s(buff, buffSize, mono_type_get_name(retType));
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

static MonoAssembly* FindAssembly(const char* assembly)
{
    MonoDomain* domain = mono_get_root_domain();
    MonoAssembly* monoAssembly = nullptr;
    auto params = std::make_pair(assembly, &monoAssembly);
    mono_assembly_foreach(AssemblyEnumerator, &params);
    return monoAssembly;
}
