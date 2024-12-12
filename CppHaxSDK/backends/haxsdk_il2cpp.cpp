//-------------------------------------------------------------------------
// [SECTION] INCLUDES
//-------------------------------------------------------------------------

#ifdef HAX_IL2CPP
#include "haxsdk_il2cpp.h"

#define WIN32_LEAN_AND_MEAN // Reduce the size of the Win32 header
#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
#include <format>

// 64 bits platforms use UnityEngine.CoreModule.dll instead of UnityEngine.dll 
// as a core library
#ifdef _WIN64
#define UNITY_CORE_ASSEMBLY "UnityEngine.CoreModule"
#else
#define UNITY_CORE_ASSEMBLY "UnityEngine"
#endif

// Custom assertion that can show chars as text.
// Default assertion takes only wide chars.
#define HAX_ASSERT(expr, msg) if (!(expr)) {                                                   \
                                 MessageBoxA(NULL, msg, "Hax assertion failed", MB_ICONERROR); \
                                 ExitProcess(-1);                                              \
                              }
// Finds IL2CPP api functions. 
// Some games use mono functions mixed with il2cpp.
// f - api function name
// i - function from il2cpp
// m - function from mono (if il2cpp function not found)
#define HAX_IL2CPP_PROC(f, i, m) f = (t ## _ ## f)(GetIl2CppProc(hGameAssembly, i, m))

//-------------------------------------------------------------------------
// [SECTION] FORWARD DECLARATIONS
//-------------------------------------------------------------------------
// 
// Types for il2cpp api functions
using t_il2cpp_domain_get                   = Il2CppDomain* (*)();
using t_il2cpp_thread_attach                = Il2CppThread* (*)(Il2CppDomain*);
using t_il2cpp_object_new                   = Il2CppObject* (*)(Il2CppClass*);
using t_il2cpp_object_unbox                 = void* (*)(Il2CppObject*);
using t_il2cpp_type_get_name                = char* (*)(Il2CppType*);
using t_il2cpp_method_get_param_count       = uint32_t (*)(Il2CppMethod*);
using t_il2cpp_method_get_param             = Il2CppType* (*)(Il2CppMethod*, uint32_t);
using t_il2cpp_method_get_name              = const char* (*)(Il2CppMethod*);
using t_il2cpp_runtime_invoke               = Il2CppObject* (*)(Il2CppMethod*, void*, void**, void*);
using t_il2cpp_class_get_type               = Il2CppType* (*)(Il2CppClass*);
using t_il2cpp_domain_assembly_open         = Il2CppAssembly* (*)(Il2CppDomain*, const char*);
using t_il2cpp_class_get_methods            = Il2CppMethod* (*)(Il2CppClass*, void**);
using t_il2cpp_class_get_field_from_name    = Il2CppClassField* (*)(Il2CppClass*, const char*);
using t_il2cpp_type_get_object              = Il2CppObject* (*)(Il2CppType*);
using t_il2cpp_class_from_name              = Il2CppClass* (*)(Il2CppImage*, const char*, const char*);
using t_il2cpp_domain_get_assemblies        = Il2CppAssembly** (*)(Il2CppDomain*, size_t*);
using t_il2cpp_string_new                   = Il2CppString* (*)(const char*);
using t_il2cpp_method_get_return_type       = Il2CppType* (*)(Il2CppMethod*);
using t_il2cpp_method_get_class             = Il2CppClass* (*)(Il2CppMethod*);
// Il2cpp api functions pointers
static t_il2cpp_domain_get                  il2cpp_domain_get;
static t_il2cpp_thread_attach               il2cpp_thread_attach;
static t_il2cpp_object_new                  il2cpp_object_new;
static t_il2cpp_object_unbox                il2cpp_object_unbox;
static t_il2cpp_type_get_name               il2cpp_type_get_name;
static t_il2cpp_method_get_param_count      il2cpp_method_get_param_count;
static t_il2cpp_method_get_param            il2cpp_method_get_param;
static t_il2cpp_method_get_name             il2cpp_method_get_name;
static t_il2cpp_method_get_class            il2cpp_method_get_class;
static t_il2cpp_runtime_invoke              il2cpp_runtime_invoke;
static t_il2cpp_class_get_type              il2cpp_class_get_type;
static t_il2cpp_domain_assembly_open        il2cpp_domain_assembly_open;
static t_il2cpp_class_get_methods           il2cpp_class_get_methods;
static t_il2cpp_class_get_field_from_name   il2cpp_class_get_field_from_name;
static t_il2cpp_type_get_object             il2cpp_type_get_object;
static t_il2cpp_class_from_name             il2cpp_class_from_name;
static t_il2cpp_domain_get_assemblies       il2cpp_domain_get_assemblies;
static t_il2cpp_string_new                  il2cpp_string_new;
static t_il2cpp_method_get_return_type      il2cpp_method_get_return_type;

static void     InitializeIl2Cpp(HMODULE hGameAssembly); // Function asserts and finds all IL2CPP api functions 
static HMODULE  GetGameAssemblyHandle();                 // Function finds handle for GameAssembly.dll. 
                                                         // It is supposed all il2cpp games use this module.
static FARPROC  GetIl2CppProc(HMODULE hGameAssembly, const char* il2cppProc, const char* monoProc); // Function finds a single IL2CPP api function.


//-------------------------------------------------------------------------
// [SECTION] FUNCTIONS DEFINITION
//-------------------------------------------------------------------------
namespace HaxSdk {
    // Function initializes everything to let user code run
    void InitializeBackendData() {
        HMODULE hGameAssembly = GetGameAssemblyHandle();
        HAX_ASSERT(hGameAssembly, "Seems like game doesnt have GameAssembly.");
        InitializeIl2Cpp(hGameAssembly);
        AttachToThread();
    }

    // Register the thread with Mono
    void AttachToThread() {
        Il2CppDomain::current()->attach_thread();
    }
}

static void InitializeIl2Cpp(HMODULE hGameAssembly) {
    HAX_IL2CPP_PROC(il2cpp_domain_get, "il2cpp_domain_get", "");
    HAX_IL2CPP_PROC(il2cpp_thread_attach, "il2cpp_thread_attach", "");
    HAX_IL2CPP_PROC(il2cpp_object_new, "il2cpp_object_new", "");
    HAX_IL2CPP_PROC(il2cpp_object_unbox, "il2cpp_object_unbox", "");
    HAX_IL2CPP_PROC(il2cpp_type_get_name, "il2cpp_type_get_name", "");
    HAX_IL2CPP_PROC(il2cpp_method_get_param_count, "il2cpp_method_get_param_count", "");
    HAX_IL2CPP_PROC(il2cpp_method_get_param, "il2cpp_method_get_param", "");
    HAX_IL2CPP_PROC(il2cpp_method_get_name, "il2cpp_method_get_name", "");
    HAX_IL2CPP_PROC(il2cpp_method_get_class, "il2cpp_method_get_class", "");
    HAX_IL2CPP_PROC(il2cpp_method_get_return_type, "il2cpp_method_get_return_type", "");
    HAX_IL2CPP_PROC(il2cpp_runtime_invoke, "il2cpp_runtime_invoke", "");
    HAX_IL2CPP_PROC(il2cpp_class_get_type, "il2cpp_class_get_type", "");
    HAX_IL2CPP_PROC(il2cpp_domain_assembly_open, "il2cpp_domain_assembly_open", "");
    HAX_IL2CPP_PROC(il2cpp_class_get_methods, "il2cpp_class_get_methods", "");
    HAX_IL2CPP_PROC(il2cpp_class_get_field_from_name, "il2cpp_class_get_field_from_name", "");
    HAX_IL2CPP_PROC(il2cpp_type_get_object, "il2cpp_type_get_object", "");
    HAX_IL2CPP_PROC(il2cpp_class_from_name, "il2cpp_class_from_name", "");
    HAX_IL2CPP_PROC(il2cpp_domain_get_assemblies, "il2cpp_domain_get_assemblies", "");
    HAX_IL2CPP_PROC(il2cpp_string_new, "il2cpp_string_new", "");
}

static HMODULE GetGameAssemblyHandle() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    MODULEENTRY32W moduleEntry = {0};
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);
    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (wcscmp(moduleEntry.szModule, L"GameAssembly.dll") == 0) {
                CloseHandle(hSnapshot);
                return moduleEntry.hModule;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }
    CloseHandle(hSnapshot);
    return 0;
}

static FARPROC GetIl2CppProc(HMODULE hGameAssembly, const char* il2cppProc, const char* monoProc) {
    FARPROC addr = GetProcAddress(hGameAssembly, il2cppProc);
    if (!addr)
        addr = GetProcAddress(hGameAssembly, monoProc);
    HAX_ASSERT(addr, il2cppProc);
    return addr;
}


//-------------------------------------------------------------------------
// [SECTION] IL2CPP STRUCTURES FUNCTIONS DEFINITION
//-------------------------------------------------------------------------
Il2CppObject* Il2CppObject::alloc(Il2CppClass* klass) {
    return il2cpp_object_new(klass);
}

void* Il2CppObject::unbox() {
    return il2cpp_object_unbox(this);
}

const char* Il2CppMethod::name() {
    return il2cpp_method_get_name(this);
}

Il2CppClass* Il2CppMethod::klass() {
    return il2cpp_method_get_class(this);
}

Il2CppObject* Il2CppMethod::invoke(void* obj, void** args) {
    return il2cpp_runtime_invoke(this, obj, args, nullptr);
}

Il2CppType* Il2CppClass::type() {
    return il2cpp_class_get_type(this);
}

Il2CppClass* Il2CppClass::find(const char* assembly, const char* name_space, const char* name) {
    return Il2CppDomain::current()->find_assembly(assembly)->find_class(name_space, name);
}

void Il2CppMethod::signature(char* buff) {
    strcpy_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_return_type(this)));
    strcat_s(buff, 255, "(");
    uint32_t nParams = il2cpp_method_get_param_count(this);
    if (nParams > 0) {
        strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param(this, 0)));
        for (uint32_t i = 1; i < nParams; ++i) {
            strcat_s(buff, 255, ",");
            strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param(this, i)));
        }
    }
    strcat_s(buff, 255, ")");
}

Il2CppMethod* Il2CppClass::find_method(const char* name, const char* sig) {
    Il2CppMethod* pMethod;
    void* iter = 0;
    char buff[255];
    while (pMethod = il2cpp_class_get_methods(this, &iter)) {
        if (strcmp(pMethod->name(), name) == 0) {
            memset(buff, 0, sizeof(buff));
            pMethod->signature(buff);
            if (strcmp(sig, buff) == 0)
                return pMethod;
        }
    }
    HAX_ASSERT(false, std::format("Method {} of {} not found in {}", name, sig, this->m_name).c_str());
    return nullptr;
}

void* Il2CppClass::find_static_field(const char* name) {
    std::cout << std::hex << offsetof(Il2CppClass, m_static_fields) << '\n';
    int offset = this->find_field(name)->offset();
    return (void*)((char*)this->m_static_fields + offset);
}

Il2CppClassField* Il2CppClass::find_field(const char* name) {
    Il2CppClassField* pField = il2cpp_class_get_field_from_name(this, name);
    HAX_ASSERT(pField, std::format("Field {} not found in {}", name, this->m_name).c_str());
    return pField;
}

Il2CppObject* Il2CppType::system_type() {
    return il2cpp_type_get_object(this);
}

Il2CppClass* Il2CppAssembly::find_class(const char* name_space, const char* name) {
    Il2CppClass* pClass = il2cpp_class_from_name(this->m_image, name_space, name);
    HAX_ASSERT(pClass, std::format("Class {}.{} not found in {}", name_space, name, this->m_name).c_str());
    return pClass;
}

Il2CppDomain* Il2CppDomain::current() {
    return il2cpp_domain_get();
}

Il2CppAssembly* Il2CppDomain::find_assembly(const char* assembly) {
    size_t nAssemblies;
    Il2CppAssembly** assemblies = il2cpp_domain_get_assemblies(Il2CppDomain::current(), &nAssemblies);
    for (int i = 0; i < nAssemblies; ++i) {
        if (strcmp(assemblies[i]->m_name, assembly) == 0) {
            return assemblies[i];
        }
    }
    HAX_ASSERT(false, std::format("Assembly {} not found", assembly).c_str());
    return nullptr;
}

void Il2CppDomain::attach_thread() {
    il2cpp_thread_attach(this);
}

Il2CppString* Il2CppString::alloc(const char* raw) {
    return il2cpp_string_new(raw);
}
#endif
