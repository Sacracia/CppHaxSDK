#ifdef HAX_MONO
#include "haxsdk_mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
#include <format>

#define HAX_ASSERT(expr, msg) if (!(expr)) {                                                   \
                                 MessageBoxA(NULL, msg, "Hax assertion failed", MB_ICONERROR); \
                                 ExitProcess(-1);                                              \
                              }
#define HAX_MONO_PROC(n)      n = reinterpret_cast<t ## _ ## n>(GetProcAddress(hMono, #n));\
                              HAX_ASSERT(n, #n)

using t_mono_get_root_domain               = MonoDomain* (*)();
using t_mono_domain_assembly_open          = MonoAssembly* (*)(MonoDomain* domain, const char* name);
using t_mono_thread_attach                 = MonoThread* (*)(MonoDomain*);
using t_mono_domain_get                    = MonoDomain* (*)();
using t_mono_string_new                    = MonoString* (*)(MonoDomain* domain, const char* string);
using t_mono_assembly_get_image            = MonoImage* (*)(MonoAssembly* assembly);
using t_mono_class_from_name               = MonoClass* (*)(MonoImage* image, const char* name_space, const char* name);
using t_mono_class_get_methods             = MonoMethod* (*)(MonoClass* klass, void** iter);
using t_mono_class_get_field_from_name     = MonoClassField* (*)(MonoClass* klass, const char* name);
using t_mono_class_vtable                  = MonoVTable* (*)(MonoDomain* domain, MonoClass* klass);
using t_mono_object_get_class              = MonoClass* (*)(MonoObject* object);
using t_mono_class_get_type                = MonoType* (*)(MonoClass* klass);
using t_mono_object_get_class              = MonoClass* (*)(MonoObject* obj);
using t_mono_method_signature              = MonoMethodSignature* (*)(MonoMethod* method);
using t_mono_method_get_name               = const char* (*)(MonoMethod* method);
using t_mono_signature_full_name           = char* (*)(MonoMethodSignature* sig);
using t_mono_compile_method                = void* (*)(MonoMethod* method);
using t_mono_jit_info_table_find           = MonoJitInfo* (*)(MonoDomain* domain, void* addr);
using t_mono_jit_info_get_method           = MonoMethod* (*)(MonoJitInfo* ji);
using t_mono_runtime_invoke                = MonoObject* (*)(MonoMethod* method, void* obj, void** params, MonoObject** exc);
using t_mono_object_unbox                  = void* (*)(MonoObject* obj);
using t_mono_vtable_get_static_field_data  = void* (*)(MonoVTable* vt);
using t_mono_type_get_object               = MonoReflectionType* (*)(MonoDomain* domain, MonoType* type);
using t_mono_object_new                    = MonoObject* (*)(MonoDomain* domain, MonoClass* klass);
using t_mono_runtime_object_init           = void (*)(MonoObject* object);

static t_mono_get_root_domain              mono_get_root_domain;
static t_mono_domain_assembly_open         mono_domain_assembly_open;
static t_mono_thread_attach                mono_thread_attach;
static t_mono_domain_get                   mono_domain_get;
static t_mono_string_new                   mono_string_new;
static t_mono_assembly_get_image           mono_assembly_get_image;
static t_mono_class_from_name              mono_class_from_name;
static t_mono_class_get_methods            mono_class_get_methods;
static t_mono_class_get_field_from_name    mono_class_get_field_from_name;
static t_mono_class_vtable                 mono_class_vtable;
static t_mono_object_get_class             mono_object_get_class;
static t_mono_class_get_type               mono_class_get_type;
static t_mono_method_signature             mono_method_signature;
static t_mono_method_get_name              mono_method_get_name;
static t_mono_signature_full_name          mono_signature_full_name;
static t_mono_compile_method               mono_compile_method;
static t_mono_jit_info_table_find          mono_jit_info_table_find;
static t_mono_jit_info_get_method          mono_jit_info_get_method;
static t_mono_runtime_invoke               mono_runtime_invoke;
static t_mono_object_unbox                 mono_object_unbox;
static t_mono_vtable_get_static_field_data mono_vtable_get_static_field_data;
static t_mono_type_get_object              mono_type_get_object;
static t_mono_object_new                   mono_object_new;
static t_mono_runtime_object_init          mono_runtime_object_init;

static void     InitializeMonoApi(HMODULE hMono);
static HMODULE  GetMonoHandle();

namespace HaxSdk {
    void InitializeMono() {
        HMODULE hMono = GetMonoHandle();
        HAX_ASSERT(hMono, "Seems like game doesnt use mono.");
        InitializeMonoApi(hMono);
        AttachToThread();
    }

    void AttachToThread() {
        mono_thread_attach(MonoDomain::root());
        mono_thread_attach(MonoDomain::current());
        std::cout << MonoDomain::root() << ' ' << MonoDomain::current() << '\n';
    }
}

static void InitializeMonoApi(HMODULE hMono) {
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
    HAX_MONO_PROC(mono_object_get_class);
    HAX_MONO_PROC(mono_class_get_type);
    HAX_MONO_PROC(mono_method_signature);
    HAX_MONO_PROC(mono_method_get_name);
    HAX_MONO_PROC(mono_signature_full_name);
    HAX_MONO_PROC(mono_compile_method);
    HAX_MONO_PROC(mono_jit_info_table_find);
    HAX_MONO_PROC(mono_jit_info_get_method);
    HAX_MONO_PROC(mono_runtime_invoke);
    HAX_MONO_PROC(mono_object_unbox);
    HAX_MONO_PROC(mono_vtable_get_static_field_data);
    HAX_MONO_PROC(mono_type_get_object);
    HAX_MONO_PROC(mono_object_new);
    HAX_MONO_PROC(mono_runtime_object_init);
}

static HMODULE GetMonoHandle() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    MODULEENTRY32W moduleEntry = {0};
    moduleEntry.dwSize = sizeof(moduleEntry);
    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (wcsncmp(moduleEntry.szModule, L"mono", 4) == 0) {
                CloseHandle(hSnapshot);
                return moduleEntry.hModule;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }
    CloseHandle(hSnapshot);
    return moduleEntry.hModule;
}

#pragma region MonoObject
inline MonoObject* MonoObject::alloc(MonoClass* klass) {
    return mono_object_new(MonoDomain::root(), klass);
}

inline void* MonoObject::unbox() {
    return mono_object_unbox(this);
}
#pragma endregion

inline MonoString* MonoString::alloc(const char* raw) {
    return mono_string_new(mono_domain_get(), raw);
}

void* MonoMethod::pointer() {
    return mono_compile_method(this);
}

inline const char* MonoMethod::name() {
    return mono_method_get_name(this);
}

inline char* MonoMethod::signature() {
    return mono_signature_full_name(mono_method_signature(this));
}

MonoClass* MonoAssembly::find_klass(const char* name_space, const char* name) {
    MonoImage* pImage = mono_assembly_get_image(this);
    MonoClass* pClass = mono_class_from_name(pImage, name_space, name);
    HAX_ASSERT(pClass, std::format("Class {}.{} not found", name_space, name).c_str());
    return pClass;
}

MonoReflectionType* MonoType::system_type() {
    return mono_type_get_object(mono_get_root_domain(), this);
}

#pragma region MonoClass
MonoClass* MonoClass::find(const char* assembly, const char* name_space, const char* name) {
    MonoAssembly* pAssembly = mono_domain_assembly_open(mono_get_root_domain(), assembly);
    HAX_ASSERT(pAssembly, std::format("Assembly {} not found", assembly).c_str());
    return pAssembly->find_klass(name_space, name);
}

inline MonoType* MonoClass::type() {
    return mono_class_get_type(this);
}

MonoMethod* MonoClass::find_method(const char* name, const char* signature) {
    void* iter = nullptr;
    MonoMethod* method;
    while (method = mono_class_get_methods(this, &iter)) {
        std::cout << method->signature() << '\n';
        if (strcmp(method->name(), name) == 0 && strcmp(method->signature(), signature) == 0) {
            return method;
        }
    }
    HAX_ASSERT(false, std::format("Method {} of {} not found in {}", name, signature, this->m_name).c_str());
    return nullptr;
}

inline MonoObject* MonoMethod::invoke(void* obj, void** args) {
    return mono_runtime_invoke(this, obj, args, nullptr);
}

void* MonoClass::find_static_field(const char* name) {
    int offset = this->find_field(name)->offset();
    MonoVTable* pVTable = mono_class_vtable(mono_domain_get(), this);
    return (void*)((char*)mono_vtable_get_static_field_data(pVTable) + offset);
}

inline MonoClassField* MonoClass::find_field(const char* name) {
    return mono_class_get_field_from_name(this, name);
}
#pragma endregion

#pragma region MonoDomain
inline MonoDomain* MonoDomain::root() {
    return mono_get_root_domain();
}

inline MonoDomain* MonoDomain::current() {
    return mono_domain_get();
}

inline void MonoDomain::attach_thread() {
    mono_thread_attach(this);
}

inline MonoAssembly* MonoDomain::assembly(const char* name) {
    return mono_domain_assembly_open(this, name);
}
#pragma endregion

#endif