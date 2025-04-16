#pragma once

#include <cstdint>

struct MonoDomain;
struct MonoAssembly;
struct MonoThread;
struct MonoImage;
struct MonoType;
struct MonoField;
struct MonoMethod;
struct MonoObject;
struct MonoString;
struct MonoClass;

using t_mono_get_root_domain                = MonoDomain* (*)();
using t_mono_domain_assembly_open           = MonoAssembly* (*)(MonoDomain*, const char*);
using t_mono_thread_attach                  = MonoThread* (*)(MonoDomain*);
using t_mono_domain_get                     = MonoDomain* (*)();
using t_mono_string_new                     = MonoString* (*)(MonoDomain*, const char*);
using t_mono_assembly_get_image             = MonoImage* (*)(MonoAssembly*);
using t_mono_class_from_name                = MonoClass * (*)(MonoImage*, const char*, const char*);
using t_mono_class_get_methods              = MonoMethod* (*)(MonoClass*, void**);
using t_mono_class_get_field_from_name      = MonoField* (*)(MonoClass*, const char*);
using t_mono_class_vtable                   = void* (*)(MonoDomain*, MonoClass*);
using t_mono_class_get_namespace            = const char* (*)(MonoClass*);
using t_mono_class_get_name                 = const char* (*)(MonoClass*);
using t_mono_object_get_class               = MonoClass * (*)(MonoObject*);
using t_mono_class_get_type                 = MonoType* (*)(MonoClass*);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject*);
using t_mono_method_signature               = void* (*)(void*);
using t_mono_method_get_name                = const char* (*)(void*);
using t_mono_compile_method                 = void* (*)(void*);
using t_mono_jit_info_table_find            = void* (*)(MonoDomain*, void*);
using t_mono_jit_info_get_method            = MonoMethod* (*)(void*);
using t_mono_runtime_invoke                 = MonoObject* (*)(void*, void*, void**, MonoObject**);
using t_mono_print_unhandled_exception      = void (*)(MonoObject*);
using t_mono_object_unbox                   = void* (*)(MonoObject*);
using t_mono_vtable_get_static_field_data   = void* (*)(void*);
using t_mono_type_get_object                = MonoType* (*)(MonoDomain*, MonoType*);
using t_mono_object_new                     = MonoObject* (*)(MonoDomain*, MonoClass*);
using t_mono_runtime_object_init            = void (*)(MonoObject*);
using t_mono_signature_get_return_type      = MonoType* (*)(void*);
using t_mono_type_get_name                  = char* (*)(MonoType*);
using t_mono_signature_get_param_count      = uint32_t (*)(void*);
using t_mono_signature_get_params           = MonoType* (*)(void*, void**);
using t_mono_method_get_unmanaged_thunk     = void* (*)(void*);
using t_mono_string_to_utf8                 = char* (*)(MonoString*);
using t_mono_field_static_get_value         = void (*)(void*, MonoField*, void*);
using t_mono_field_static_set_value         = void (*)(void*, MonoField*, void*);
using t_mono_thread_detach                  = void (*)(MonoThread*);

inline t_mono_get_root_domain               mono_get_root_domain;
inline t_mono_domain_assembly_open          mono_domain_assembly_open;
inline t_mono_thread_attach                 mono_thread_attach;
inline t_mono_domain_get                    mono_domain_get;
inline t_mono_string_new                    mono_string_new;
inline t_mono_assembly_get_image            mono_assembly_get_image;
inline t_mono_class_from_name               mono_class_from_name;
inline t_mono_class_get_methods             mono_class_get_methods;
inline t_mono_class_get_field_from_name     mono_class_get_field_from_name;
inline t_mono_class_get_namespace           mono_class_get_namespace;
inline t_mono_class_get_name                mono_class_get_name;
inline t_mono_class_vtable                  mono_class_vtable;
inline t_mono_object_get_class              mono_object_get_class;
inline t_mono_class_get_type                mono_class_get_type;
inline t_mono_method_signature              mono_method_signature;
inline t_mono_method_get_name               mono_method_get_name;
inline t_mono_compile_method                mono_compile_method;
inline t_mono_jit_info_table_find           mono_jit_info_table_find;
inline t_mono_jit_info_get_method           mono_jit_info_get_method;
inline t_mono_runtime_invoke                mono_runtime_invoke;
inline t_mono_object_unbox                  mono_object_unbox;
inline t_mono_vtable_get_static_field_data  mono_vtable_get_static_field_data;
inline t_mono_type_get_object               mono_type_get_object;
inline t_mono_object_new                    mono_object_new;
inline t_mono_runtime_object_init           mono_runtime_object_init;
inline t_mono_signature_get_return_type     mono_signature_get_return_type;
inline t_mono_type_get_name                 mono_type_get_name;
inline t_mono_signature_get_param_count     mono_signature_get_param_count;
inline t_mono_signature_get_params          mono_signature_get_params;
inline t_mono_method_get_unmanaged_thunk    mono_method_get_unmanaged_thunk;
inline t_mono_string_to_utf8                mono_string_to_utf8;
inline t_mono_field_static_get_value        mono_field_static_get_value;
inline t_mono_field_static_set_value        mono_field_static_set_value;
inline t_mono_thread_detach                 mono_thread_detach;

namespace Mono
{
	void Initialize();
	void AttachThread();
}