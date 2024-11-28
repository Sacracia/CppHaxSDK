#pragma once
#ifndef HAX_API
#define HAX_API
#endif

#include <stdint.h>
#include <assert.h>

#include "../logger/logger.h"
#include "../gui/gui.h"

/*---------------------------------------------------------------------------- -
Forward declarations
-----------------------------------------------------------------------------*/
#pragma region Structs
struct MonoThread;
struct MonoDomain;
struct MonoAssembly;
struct MonoType;
struct MonoClass;
struct MonoImage;
struct MonoMethod;
struct MonoString;
struct MonoVTable;
struct MonoThreadsSync;
struct MonoJitInfo;
struct MonoError;
struct MonoObject;
struct MonoClassField;
struct MonoReflectionType;
struct MonoArrayBounds;
template <typename T> struct MonoArray;

// Representation of .NET classes from mscorlib
using String = MonoString;
using Type = MonoReflectionType;
template <typename T> using Array = MonoArray<T>;
template <typename T> struct List;

// Unity core classes
struct Object;
struct Vector3;
struct Transfrom;
struct Camera;

// Custom
struct MonoMethodWrapper;
#pragma endregion


#pragma region MonoApiFunctions
#ifdef HAX_API
HAX_API MonoDomain*		(*mono_get_root_domain)				();
HAX_API MonoAssembly*	(*mono_domain_assembly_open)		(MonoDomain* domain, const char* name);
HAX_API MonoThread*		(*mono_thread_attach)				(MonoDomain* domain);
HAX_API MonoDomain*		(*mono_domain_get)					();
HAX_API MonoString*     (*mono_string_new)                  (MonoDomain* domain, const char* string);
// image
HAX_API MonoImage*		(*mono_assembly_get_image)			(MonoAssembly* assembly);
// class
HAX_API MonoClass*		(*mono_class_from_name)				(MonoImage* image, const char* name_space, const char* name);
HAX_API MonoMethod*		(*mono_class_get_methods)			(MonoClass* klass, void** iter);
HAX_API MonoClassField*	(*mono_class_get_field_from_name)	(MonoClass* klass, const char* name);
HAX_API MonoVTable*		(*mono_class_vtable)				(MonoDomain* domain, MonoClass* klass);
HAX_API MonoClass*		(*mono_object_get_class)			(MonoObject* object);
HAX_API MonoType*       (*mono_class_get_type)              (MonoClass* klass);
// method
HAX_API const char*		(*mono_method_full_name)			(MonoMethod* method, int32_t signature);
HAX_API void*			(*mono_compile_method)				(MonoMethod* method);
HAX_API MonoJitInfo*	(*mono_jit_info_table_find)			(MonoDomain* domain, void* addr);
HAX_API MonoMethod*		(*mono_jit_info_get_method)			(MonoJitInfo* ji);
HAX_API MonoObject*		(*mono_runtime_invoke)				(MonoMethod* method, void* obj, void** params, MonoObject** exc);
HAX_API void*			(*mono_object_unbox)				(MonoObject* obj);
// field
HAX_API void*			    (*mono_vtable_get_static_field_data)    (MonoVTable* vt);
HAX_API MonoReflectionType* (*mono_type_get_object)                 (MonoDomain* domain, MonoType* type);
#endif
#pragma endregion
 

namespace HaxSdk {
	void                InitializeMono              ();
}

#pragma region StructsDefinitions
struct MonoObject {
	MonoVTable*         vtable;
	MonoThreadsSync*    synchronisation;
};

struct MonoClassField {
	MonoType*           type;
	const char*         name;
	MonoClass*          parent;
	int                 offset;
};

struct MonoArrayBounds {
	uint64_t            length;
	int64_t             lower_bound;
};

struct MonoMethodWrapper {
    explicit operator bool() const { 
        return method && ptr; 
    }
    MonoMethod*         method;
    void*               ptr;
};

struct MonoString {
    static MonoString* New(const char* content) { 
        return mono_string_new(mono_domain_get(), content); 
    }
    MonoObject          object;
    int32_t             length;
    wchar_t             chars[32];
};

template <class T>
struct MonoArray {
    MonoObject          object;
    MonoArrayBounds*    bounds;
    uint64_t            max_length;
    T                   vector[32];
};

struct MonoClass {
    MonoMethodWrapper GetMethod(const char* signature);
    Type* GetType() {
        return mono_type_get_object(mono_domain_get(), mono_class_get_type(this));
    }
    MonoClassField* Field(const char* name) {
        return mono_class_get_field_from_name(this, name);
    }
    void* StaticField(const char* name) {
        MonoVTable* pVTable = mono_class_vtable(mono_domain_get(), this);
        char* pData = (char*)mono_vtable_get_static_field_data(pVTable);
        return (void*)(pData + this->Field(name)->offset);
    }
};

struct MonoDomain {
    static MonoDomain* Current() {
        return mono_domain_get();
    }
    MonoVTable* VTable(MonoClass* pClass) {
        return mono_class_vtable(this, pClass);
    }
    MonoAssembly* OpenAssembly(const char* assembly) {
        return mono_domain_assembly_open(this, assembly);
    }
    void AttachThread() {
        mono_thread_attach(this);
    }
};

struct MonoAssembly {
    MonoImage* GetImage(const char* name) {
        return mono_assembly_get_image(this);
    }
};

struct MonoImage {
    MonoClass* GetClass(const char* name_space, const char* name) {
        return mono_class_from_name(this, name_space, name);
    }
};

template <class T>
struct List {
    MonoObject          object;
    Array<T>*           _items;
    int32_t             _size;
    int32_t             _version;
    void*               _syncRoot;
};

struct Object {
    static Array<MonoObject*>* FindObjectsOfType(MonoType* type);
};

struct Vector3 {
    float               x;
    float               y;
    float               z;
};

struct Transform {
    Vector3 get_position();
};

struct Camera {
    Vector3 WorldToScreenPoint(Vector3 position);
};
#pragma endregion