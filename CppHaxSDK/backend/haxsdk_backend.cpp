#include "haxsdk_backend.h"

struct MonoDomain;
struct MonoAssembly;
struct MonoThread;
struct MonoImage;
struct MonoType;
struct MonoField;
struct MonoMethod;
struct MonoObject;
struct MonoString;

using t_mono_get_root_domain                = MonoDomain* (*)();
using t_mono_domain_assembly_open           = MonoAssembly* (*)(MonoDomain*, const char*);
using t_mono_thread_attach                  = MonoThread* (*)(MonoDomain*);
using t_mono_domain_get                     = MonoDomain* (*)();
using t_mono_string_new                     = MonoString* (*)(MonoDomain*, const char*);
using t_mono_assembly_get_image             = MonoImage* (*)(MonoAssembly*);
using t_mono_class_from_name                = Class* (*)(MonoImage*, const char*, const char*);
using t_mono_class_get_methods              = MonoMethod* (*)(Class*, void**);
using t_mono_class_get_field_from_name      = MonoField* (*)(Class*, const char*);
using t_mono_class_vtable                   = void* (*)(MonoDomain*, Class*);
using t_mono_class_get_namespace            = const char* (*)(Class*);
using t_mono_class_get_name                 = const char* (*)(Class*);
using t_mono_object_get_class               = Class* (*)(MonoObject*);
using t_mono_class_get_type                 = MonoType* (*)(Class*);
using t_mono_object_get_class               = Class* (*)(MonoObject*);
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
using t_mono_object_new                     = MonoObject* (*)(MonoDomain*, Class*);
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

namespace Mono
{
	
}

bool Class::Exists(const char* assembly, const char* nameSpace, const char* name, Class** out)
{
	
}

Class* Class::Find(const char* assembly, const char* nameSpace, const char* name)
{

}

