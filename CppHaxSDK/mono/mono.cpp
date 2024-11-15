#include "mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <sstream>
#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR) assert(_EXPR)
#endif

#include "../logger/logger.h"
#include "mono_api_classes.h"
#define MONO_API_FUNC(r, n, p) r(*n)p
#include "mono_api_functions.h"
#undef MONO_API_FUNC

MonoDomain* domain;

static HMODULE      GetMonoHandle();
static void         InitMonoApi(HMODULE hMono);
static MonoClass*   GetMonoClass(const char* assemblyName, const char* name_space, const char* klassName);

int MonoObject::GetFieldOffset(MonoClass*& klass, const char* fieldName) {
    if (!klass) {
        klass = mono_object_get_class(this);
    }
    return mono_class_get_field_from_name(klass, fieldName)->offset;
}

void* MonoObject::GetStaticField(MonoClass*& klass, const char* assemblyName, const char* name_space,
                                 const char* klassName, const char* fieldName) {
    if (!klass) {
        klass = GetMonoClass(assemblyName, name_space, klassName);
    }
    MonoVTable* vtable = mono_class_vtable(domain, klass);
    MonoClassField* field = mono_class_get_field_from_name(klass, fieldName);
    void* ptr = mono_vtable_get_static_field_data(vtable);
    return (void*)((char*)mono_vtable_get_static_field_data(vtable) + field->offset);
}

namespace mono {
    void Initialize() {
        HMODULE hMono = GetMonoHandle();
        HAX_ASSERT(hMono != NULL && "Game not using mono");

        InitMonoApi(hMono);

        domain = mono_get_root_domain();
        mono_thread_attach(domain);
        mono_thread_attach(mono_domain_get());
    }

    void* GetFuncAddress(std::string signature) {
        std::string assemblyName = signature.substr(0, signature.find(", "));
        signature.erase(0, assemblyName.length() + 2);
        std::string sig = signature;
        std::string name_space = "";
        if (auto pos = signature.find("."); pos != std::string::npos) {
            name_space = signature.substr(0, pos);
            signature.erase(0, name_space.length() + 1);
        }
        std::string klassName = signature.substr(0, signature.find(':'));
        MonoClass* klass = GetMonoClass(assemblyName.c_str(), name_space.c_str(), klassName.c_str());

        void* iter = nullptr;
        MonoMethod* method;
        while (method = mono_class_get_methods(klass, &iter)) {
            if (sig == mono_method_full_name(method, 1)) {
                return mono_compile_method(method);
            }
        }

        return nullptr;
    }
}

static void InitMonoApi(HMODULE hMono) {
    #define MONO_API_FUNC(r, n, p)                          \
        n = (r(*)p)(GetProcAddress(hMono, #n));             \
        HAX_ASSERT(n && #n " not found")
    #include "mono_api_functions.h"
    #undef MONO_API_FUNC
}

static HMODULE GetMonoHandle() {
	DWORD processId = GetProcessId(GetCurrentProcess());
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

	MODULEENTRY32 me{};
	me.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(snapshot, &me)) {
		do {
            if (GetProcAddress(me.hModule, "mono_thread_attach")) {
				CloseHandle(snapshot);
				return me.hModule;
			}
		} while (Module32Next(snapshot, &me));
	}

	CloseHandle(snapshot);
	return NULL;
}

static MonoClass* GetMonoClass(const char* assemblyName, const char* name_space, const char* klassName) {
    MonoAssembly* assembly = mono_domain_assembly_open(domain, assemblyName);
    HAX_ASSERT(assembly != nullptr && "mono_domain_assembly_open failed");
    MonoImage* image = mono_assembly_get_image(assembly);
    HAX_ASSERT(image != nullptr && "mono_assembly_get_image failed");
    MonoClass* klass = mono_class_from_name(image, name_space, klassName);
    HAX_ASSERT(klass != nullptr && "mono_class_from_name failed");
    return klass;
}