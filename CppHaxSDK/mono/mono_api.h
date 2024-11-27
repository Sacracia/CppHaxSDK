#pragma once
#ifndef HAX_API
#define HAX_API
#endif

#include <stdint.h>

#include "../logger/logger.h"
#include "../gui/gui.h"

// Forward declarations
struct MonoThread;
struct MonoDomain;
struct MonoAssembly;
struct MonoType;
struct MonoClass;
struct MonoImage;
struct MonoMethod;
struct MonoString;
struct MonoVTable;
struct MonoArray;
struct MonoThreadsSync;
struct MonoJitInfo;
struct MonoError;
struct MonoObject;
struct MonoClassField;
struct MonoArrayBounds;

#ifdef HAX_API
HAX_API MonoDomain*		(*mono_get_root_domain)				();
HAX_API MonoAssembly*	(*mono_domain_assembly_open)		(MonoDomain* domain, const char* name);
HAX_API MonoThread*		(*mono_thread_attach)				(MonoDomain* domain);
HAX_API MonoDomain*		(*mono_domain_get)					();
// image
HAX_API MonoImage*		(*mono_assembly_get_image)			(MonoAssembly* assembly);
// class
HAX_API MonoClass*		(*mono_class_from_name)				(MonoImage* image, const char* name_space, const char* name);
HAX_API MonoMethod*		(*mono_class_get_methods)			(MonoClass* klass, void** iter);
HAX_API MonoClassField*	(*mono_class_get_field_from_name)	(MonoClass* klass, const char* name);
HAX_API MonoVTable*		(*mono_class_vtable)				(MonoDomain* domain, MonoClass* klass);
HAX_API MonoClass*		(*mono_object_get_class)			(MonoObject* object);
// method
HAX_API const char*		(*mono_method_full_name)			(MonoMethod* method, int32_t signature);
HAX_API void*			(*mono_compile_method)				(MonoMethod* method);
HAX_API MonoJitInfo*	(*mono_jit_info_table_find)			(MonoDomain* domain, void* addr);
HAX_API MonoMethod*		(*mono_jit_info_get_method)			(MonoJitInfo* ji);
HAX_API MonoObject*		(*mono_runtime_invoke)				(MonoMethod* method, void* obj, void** params, MonoObject** exc);
HAX_API void*			(*mono_object_unbox)				(MonoObject* obj);
// field
HAX_API void*			(*mono_vtable_get_static_field_data)(MonoVTable* vt);
#endif

namespace HaxSdk {
	void InitializeMono();
}

struct MonoObject {
	MonoVTable* vtable;
	MonoThreadsSync* synchronisation;
};

struct MonoClassField {
	MonoType* type;
	const char* name;
	MonoClass* parent;
	int offset;
};

struct MonoArrayBounds {
	uint64_t length;
	int64_t lower_bound;
};

struct Vector3 {
	float x;
	float y;
	float z;
};