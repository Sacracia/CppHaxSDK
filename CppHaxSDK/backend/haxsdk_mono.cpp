#include "haxsdk_mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "../haxsdk.h"

#define HAX_MONO_PROC(n)      n = reinterpret_cast<t ## _ ## n>(GetProcAddress(handle, #n));\
                              HAX_ASSERT(n, #n)
#define HAX_MONO_PROC_OPT(n)  n = reinterpret_cast<t ## _ ## n>(GetProcAddress(handle, #n));\

void Mono::Initialize() 
{
    HMODULE handle = (HMODULE)HaxSdk::GetGlobals().backendHandle;
    HAX_MONO_PROC(mono_get_root_domain);
    HAX_MONO_PROC(mono_domain_assembly_open);
    HAX_MONO_PROC(mono_thread_attach);
    HAX_MONO_PROC(mono_domain_get);
    HAX_MONO_PROC(mono_string_new);
    HAX_MONO_PROC(mono_assembly_get_image);
    HAX_MONO_PROC(mono_class_from_name);
    HAX_MONO_PROC(mono_class_get_methods);
    HAX_MONO_PROC(mono_class_get_field_from_name);
    HAX_MONO_PROC(mono_class_vtable);
    HAX_MONO_PROC(mono_class_get_name);
    HAX_MONO_PROC(mono_class_get_namespace);
    HAX_MONO_PROC(mono_object_get_class);
    HAX_MONO_PROC(mono_class_get_type);
    HAX_MONO_PROC(mono_method_signature);
    HAX_MONO_PROC(mono_method_get_name);
    HAX_MONO_PROC(mono_compile_method);
    HAX_MONO_PROC(mono_jit_info_table_find);
    HAX_MONO_PROC(mono_jit_info_get_method);
    HAX_MONO_PROC(mono_runtime_invoke);
    HAX_MONO_PROC(mono_object_unbox);
    HAX_MONO_PROC(mono_vtable_get_static_field_data);
    HAX_MONO_PROC(mono_type_get_object);
    HAX_MONO_PROC(mono_object_new);
    HAX_MONO_PROC(mono_runtime_object_init);
    HAX_MONO_PROC(mono_signature_get_return_type);
    HAX_MONO_PROC(mono_type_get_name);
    HAX_MONO_PROC(mono_signature_get_param_count);
    HAX_MONO_PROC(mono_signature_get_params);
    HAX_MONO_PROC(mono_string_to_utf8);
    HAX_MONO_PROC_OPT(mono_method_get_unmanaged_thunk);
}

void Mono::AttachThread()
{

}