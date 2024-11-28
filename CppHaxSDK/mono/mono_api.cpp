#include "mono_api.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <assert.h>
#include <iostream>

#ifdef _WIN64
#define UNITY_CORE_ASSEMBLY "UnityEngine.CoreModule"
#else
#define UNITY_CORE_ASSEMBLY "UnityEngine"
#endif

#define MONO_FUNCTION(a, n, c, m, s) MonoMethodWrapper c ## __ ## m
#define MONO_STATIC_FIELD(a, n, c, f, t) t* c ## __ ## f
#include "core_data.h"
#include "../cheat/game_data.h"
#undef MONO_FUNCTION
#undef MONO_STATIC_FIELD

static MonoDomain* g_domain;

static HMODULE GetMonoHandle();
static void InitializeMonoApiFunctions(HMODULE hMono);
static void InitializeCoreData();

namespace HaxSdk {
	void InitializeMono() {
		HMODULE hMono = GetMonoHandle();
		assert(hMono && "Mono not found");
		InitializeMonoApiFunctions(hMono);
		HaxSdk::AttachToThread();
        InitializeCoreData();
	}

	void AttachToThread() {
        mono_get_root_domain()->AttachThread();
        mono_domain_get()->AttachThread();
	}
}

static HMODULE GetMonoHandle() {
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
	MODULEENTRY32 me{};
	me.dwSize = sizeof(MODULEENTRY32);
	if (Module32First(snapshot, &me)) {
		do {
			if (strncmp(me.szModule, "mono", 4) == 0) {
				CloseHandle(snapshot);
				return me.hModule;
			}
		} while (Module32Next(snapshot, &me));
	}

	CloseHandle(snapshot);
	return NULL;
}

static void InitializeMonoApiFunctions(HMODULE hMono) {
	assert(mono_get_root_domain = reinterpret_cast<decltype(mono_get_root_domain)>(GetProcAddress(hMono, "mono_get_root_domain")));
	assert(mono_domain_assembly_open = reinterpret_cast<decltype(mono_domain_assembly_open)>(GetProcAddress(hMono, "mono_domain_assembly_open")));
	assert(mono_thread_attach = reinterpret_cast<decltype(mono_thread_attach)>(GetProcAddress(hMono, "mono_thread_attach")));
	assert(mono_domain_get = reinterpret_cast<decltype(mono_domain_get)>(GetProcAddress(hMono, "mono_domain_get")));
	assert(mono_assembly_get_image = reinterpret_cast<decltype(mono_assembly_get_image)>(GetProcAddress(hMono, "mono_assembly_get_image")));
	assert(mono_class_from_name = reinterpret_cast<decltype(mono_class_from_name)>(GetProcAddress(hMono, "mono_class_from_name")));
	assert(mono_class_get_methods = reinterpret_cast<decltype(mono_class_get_methods)>(GetProcAddress(hMono, "mono_class_get_methods")));
	assert(mono_class_get_field_from_name = reinterpret_cast<decltype(mono_class_get_field_from_name)>(GetProcAddress(hMono, "mono_class_get_field_from_name")));
	assert(mono_class_vtable = reinterpret_cast<decltype(mono_class_vtable)>(GetProcAddress(hMono, "mono_class_vtable")));
	assert(mono_object_get_class = reinterpret_cast<decltype(mono_object_get_class)>(GetProcAddress(hMono, "mono_object_get_class")));
    assert(mono_class_get_type = reinterpret_cast<decltype(mono_class_get_type)>(GetProcAddress(hMono, "mono_class_get_type")));
	// method
	assert(mono_method_full_name = reinterpret_cast<decltype(mono_method_full_name)>(GetProcAddress(hMono, "mono_method_full_name")));
	assert(mono_compile_method = reinterpret_cast<decltype(mono_compile_method)>(GetProcAddress(hMono, "mono_compile_method")));
	assert(mono_jit_info_table_find = reinterpret_cast<decltype(mono_jit_info_table_find)>(GetProcAddress(hMono, "mono_jit_info_table_find")));
	assert(mono_jit_info_get_method = reinterpret_cast<decltype(mono_jit_info_get_method)>(GetProcAddress(hMono, "mono_jit_info_get_method")));
	assert(mono_runtime_invoke = reinterpret_cast<decltype(mono_runtime_invoke)>(GetProcAddress(hMono, "mono_runtime_invoke")));
	assert(mono_object_unbox = reinterpret_cast<decltype(mono_object_unbox)>(GetProcAddress(hMono, "mono_object_unbox")));
	// field
	assert(mono_vtable_get_static_field_data = reinterpret_cast<decltype(mono_vtable_get_static_field_data)>(GetProcAddress(hMono, "mono_vtable_get_static_field_data")));
    assert(mono_string_new = reinterpret_cast<decltype(mono_string_new)>(GetProcAddress(hMono, "mono_string_new")));
    assert(mono_type_get_object = reinterpret_cast<decltype(mono_type_get_object)>(GetProcAddress(hMono, "mono_type_get_object")));
}

static void InitializeCoreData() {
    #define MONO_STATIC_FIELD(a, n, c, f, t)  c ## __ ## f = (t*)HaxSdk::GetUnityStaticFieldAddress(a, n, #c, #f);  \
                                              std::cout << #c "__" #f ": " << c ## __ ## f << '\n';         \
                                              assert(c ## __ ## f)
    #define MONO_FUNCTION(a, n, c, m, s)  c ## __ ## m = HaxSdk::GetUnityFunction(a, n, #c, #m " " s); \
                                          std::cout << #c "__" #m ": " << c ## __ ## m ## .ptr << '\n';       \
                                          assert(c ## __ ## m)
    #include "core_data.h"
    #include "../cheat/game_data.h"
}

Vector3 Transform::get_position() {
    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(Transform__get_position.method, this, nullptr, nullptr));
}

Vector3 Camera::WorldToScreenPoint(Vector3 position) {
    void* args[1] = { &position };
    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(Camera__WorldToScreenPoint.method, this, args, nullptr));
}

Array<MonoObject*>* Object::FindObjectsOfType(MonoType* type) {
    MonoReflectionType* systemType = mono_type_get_object(g_domain, type);
    return ((Array<MonoObject*>*(__fastcall*)(MonoReflectionType*))Object__FindObjectsOfType.ptr)(systemType);
}

MonoMethodWrapper MonoClass::GetMethod(const char* signature) {
    void* iter = nullptr;
    MonoMethod* method;
    while (method = mono_class_get_methods(this, &iter)) {
        if (strcmp(signature, mono_method_full_name(method, 1)) == 0) {
            return { method, mono_compile_method(method) };
        }
    }

    char message[255];
    strcpy(message, "Method not found ");
    strcat(message, signature);
    throw std::runtime_error(message);
}
