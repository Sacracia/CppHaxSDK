#include "mono_api.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <assert.h>
#include <iostream>

static MonoDomain* g_domain;

static HMODULE GetMonoHandle();
static void InitializeMonoApiFunctions(HMODULE hMono);

namespace HaxSdk {
	void InitializeMono() {
		HMODULE hMono = GetMonoHandle();
		assert(hMono && "Mono not found");
		InitializeMonoApiFunctions(hMono);
		HaxSdk::AttachToThread();
	}

	void AttachToThread() {
		g_domain = mono_get_root_domain();
		mono_thread_attach(g_domain);
		mono_thread_attach(mono_domain_get());
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
	// method
	assert(mono_method_full_name = reinterpret_cast<decltype(mono_method_full_name)>(GetProcAddress(hMono, "mono_method_full_name")));
	assert(mono_compile_method = reinterpret_cast<decltype(mono_compile_method)>(GetProcAddress(hMono, "mono_compile_method")));
	assert(mono_jit_info_table_find = reinterpret_cast<decltype(mono_jit_info_table_find)>(GetProcAddress(hMono, "mono_jit_info_table_find")));
	assert(mono_jit_info_get_method = reinterpret_cast<decltype(mono_jit_info_get_method)>(GetProcAddress(hMono, "mono_jit_info_get_method")));
	assert(mono_runtime_invoke = reinterpret_cast<decltype(mono_runtime_invoke)>(GetProcAddress(hMono, "mono_runtime_invoke")));
	assert(mono_object_unbox = reinterpret_cast<decltype(mono_object_unbox)>(GetProcAddress(hMono, "mono_object_unbox")));
	// field
	assert(mono_vtable_get_static_field_data = reinterpret_cast<decltype(mono_vtable_get_static_field_data)>(GetProcAddress(hMono, "mono_vtable_get_static_field_data")));
}

