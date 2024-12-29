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

using MethodSignature = void;

using t_mono_get_root_domain               = Domain* (*)();
using t_mono_domain_assembly_open          = Assembly* (*)(Domain*, const char*);
using t_mono_thread_attach                 = Thread* (*)(Domain*);
using t_mono_domain_get                    = Domain* (*)();
using t_mono_string_new                    = System::String* (*)(Domain*, const char*);
using t_mono_assembly_get_image            = Image* (*)(Assembly*);
using t_mono_class_from_name               = Class* (*)(Image*, const char*, const char*);
using t_mono_class_get_methods             = Method* (*)(Class*, void**);
using t_mono_class_get_field_from_name     = Field* (*)(Class*, const char*);
using t_mono_class_vtable                  = void* (*)(Domain*, Class*);
using t_mono_class_get_namespace           = const char* (*)(Class*);
using t_mono_class_get_name                = const char* (*)(Class*);
using t_mono_object_get_class              = Class* (*)(System::Object*);
using t_mono_class_get_type                = Type* (*)(Class*);
using t_mono_object_get_class              = Class* (*)(System::Object*);
using t_mono_method_signature              = MethodSignature* (*)(void*);
using t_mono_method_get_name               = const char* (*)(void*);
using t_mono_compile_method                = void* (*)(void*);
using t_mono_jit_info_table_find           = void* (*)(Domain*, void*);
using t_mono_jit_info_get_method           = Method* (*)(void*);
using t_mono_runtime_invoke                = System::Object* (*)(void*, void*, void**, System::Object**);
using t_mono_print_unhandled_exception     = void (*)(System::Object*);
using t_mono_object_unbox                  = void* (*)(System::Object*);
using t_mono_vtable_get_static_field_data  = void* (*)(void*);
using t_mono_type_get_object               = System::Type* (*)(Domain*, Type*);
using t_mono_object_new                    = System::Object* (*)(Domain*, Class*);
using t_mono_runtime_object_init           = void (*)(System::Object*);
using t_mono_signature_get_return_type     = Type* (*)(MethodSignature*);
using t_mono_type_get_name                 = char* (*)(Type*);
using t_mono_signature_get_param_count     = uint32_t (*)(MethodSignature*);
using t_mono_signature_get_params          = Type* (*)(MethodSignature*, void**);
using t_mono_method_get_unmanaged_thunk    = void* (*)(void*);
using t_mono_string_to_utf8                = char* (*)(System::String*);

static t_mono_get_root_domain              mono_get_root_domain;
static t_mono_domain_assembly_open         mono_domain_assembly_open;
static t_mono_thread_attach                mono_thread_attach;
static t_mono_domain_get                   mono_domain_get;
static t_mono_string_new                   mono_string_new;
static t_mono_assembly_get_image           mono_assembly_get_image;
static t_mono_class_from_name              mono_class_from_name;
static t_mono_class_get_methods            mono_class_get_methods;
static t_mono_class_get_field_from_name    mono_class_get_field_from_name;
static t_mono_class_get_namespace          mono_class_get_namespace;
static t_mono_class_get_name               mono_class_get_name;
static t_mono_class_vtable                 mono_class_vtable;
static t_mono_object_get_class             mono_object_get_class;
static t_mono_class_get_type               mono_class_get_type;
static t_mono_method_signature             mono_method_signature;
static t_mono_method_get_name              mono_method_get_name;
static t_mono_compile_method               mono_compile_method;
static t_mono_jit_info_table_find          mono_jit_info_table_find;
static t_mono_jit_info_get_method          mono_jit_info_get_method;
static t_mono_runtime_invoke               mono_runtime_invoke;
static t_mono_object_unbox                 mono_object_unbox;
static t_mono_vtable_get_static_field_data mono_vtable_get_static_field_data;
static t_mono_type_get_object              mono_type_get_object;
static t_mono_object_new                   mono_object_new;
static t_mono_runtime_object_init          mono_runtime_object_init;
static t_mono_signature_get_return_type    mono_signature_get_return_type;
static t_mono_type_get_name                mono_type_get_name;
static t_mono_signature_get_param_count    mono_signature_get_param_count;
static t_mono_signature_get_params         mono_signature_get_params;
static t_mono_method_get_unmanaged_thunk   mono_method_get_unmanaged_thunk;
static t_mono_string_to_utf8               mono_string_to_utf8;

static void     InitializeMono(HMODULE hMono);
static HMODULE  GetMonoHandle();

namespace HaxSdk {
    void InitializeBackendData() {
        HMODULE hMono = GetMonoHandle();
        HAX_ASSERT(hMono, "Seems like game doesnt use mono.");
        InitializeMono(hMono);
        AttachToThread();
    }

    void AttachToThread() {
        Domain::Root()->AttachThread();
    }
}

static void InitializeMono(HMODULE hMono) {
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
    mono_method_get_unmanaged_thunk = (t_mono_method_get_unmanaged_thunk)GetProcAddress(hMono, "mono_method_get_unmanaged_thunk");  // may not exist
}

static HMODULE GetMonoHandle() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    MODULEENTRY32W moduleEntry = { 0 };
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
    return 0;
}


// Dedinition: System::Object
System::Object* System::Object::New(Class* pClass) { 
    return mono_object_new(Domain::Root(), pClass); 
}

System::Object* System::Object::NewI(Class* pClass) {
    System::Object* pObj = mono_object_new(Domain::Root(), pClass);
    mono_runtime_object_init(pObj);
    return pObj;
}

void* System::Object::Unbox() { 
    return mono_object_unbox(this); 
}


// Dedinition: System::String
System::String* System::String::New(const char* data) { 
    return mono_string_new(Domain::Root(), data); 
}

System::String* System::String::Concat(System::String* s1, System::String* s2) {
    static Method pMethod = Class::Find("mscorlib", "System", "String")->FindMethod("Concat", "System.String(System.String,System.String)");
    void* args[] = { s1, s2 };
    return (System::String*)pMethod.Invoke(nullptr, args);
}

char* System::String::UTF8() { 
    return mono_string_to_utf8(this); 
}

// Dedinition: Domain
Domain* Domain::Root() {
    return mono_get_root_domain();
}

void Domain::AttachThread() {
    mono_thread_attach(this);
}

Assembly* Domain::FindAssembly(const char* name) {
    Assembly* pAssembly = mono_domain_assembly_open(this, name);
    HAX_ASSERT(pAssembly, std::format("Assembly {} not found", name).c_str());
    return pAssembly;
}

bool Domain::HasAssembly(const char* name) {
    return mono_domain_assembly_open(this, name) != nullptr;
}


// Dedinition: Assembly
Class* Assembly::FindClass(const char* name_space, const char* name) {
    Image* pImage = mono_assembly_get_image(this);
    Class* pClass = mono_class_from_name(pImage, name_space, name);
    HAX_ASSERT(pClass, std::format("Class {}.{} not found", name_space, name).c_str());
    return pClass;
}


// Dedinition: Class
Class* Class::Find(const char* assembly, const char* name_space, const char* name) {
    Assembly* pAssembly = mono_domain_assembly_open(mono_get_root_domain(), assembly);
    HAX_ASSERT(pAssembly, std::format("Assembly {} not found", assembly).c_str());
    return pAssembly->FindClass(name_space, name);
}

System::Type* Class::Type() { 
    return mono_class_get_type(this)->SystemType(); 
}

const char* Class::Name() { 
    return mono_class_get_name(this); 
}

const char* Class::Namespace() { 
    return mono_class_get_namespace(this); 

}

Method Class::FindMethod(const char* name, const char* sig) {
    void* iter = nullptr;
    void* pMethod;
    char buff[255];
    while (pMethod = mono_class_get_methods(this, &iter)) {
        Method method = { (Method::MonoMethod*)pMethod, nullptr };
        if (strcmp(mono_method_get_name(pMethod), name) == 0) {
            memset(buff, 0, 255);
            method.Signature(buff);
            if (strcmp(buff, sig) == 0) {
                method.m_ptr = mono_method_get_unmanaged_thunk ? mono_method_get_unmanaged_thunk(pMethod) : mono_compile_method(pMethod);
                return method;
            }
        }
    }
    HAX_ASSERT(false, std::format("Method {} of {} not found in {}", name, sig, this->Name()).c_str());
    return { nullptr, nullptr };
}

Method Class::FindMethod(const char* name) {
    void* iter = nullptr;
    void* pMethod;
    while (pMethod = mono_class_get_methods(this, &iter)) {
        Method method = { (Method::MonoMethod*)pMethod, nullptr };
        if (strcmp(mono_method_get_name(pMethod), name) == 0) {
            method.m_ptr = mono_method_get_unmanaged_thunk ? mono_method_get_unmanaged_thunk(pMethod) : mono_compile_method(pMethod);
            return method;
        }
    }

    HAX_ASSERT(false, std::format("Method {} not found in {}", name, this->Name()).c_str());
    return { nullptr, nullptr };
}

void* Class::FindStaticField(const char* name) {
    int32_t offset = this->FindField(name)->Offset();
    void* pVTable = mono_class_vtable(mono_domain_get(), this);
    return (void*)((char*)mono_vtable_get_static_field_data(pVTable) + offset);
}

Field* Class::FindField(const char* name) {
    Field* pField = mono_class_get_field_from_name(this, name);
    HAX_ASSERT(pField, std::format("Field {} not found in class {}", name, this->Name()).c_str());
    return pField;
}


// Dedinition: Method
System::Object* Method::Invoke(void* __this, void** args) { 
    return mono_runtime_invoke(m_base, __this, args, nullptr); 
}

const char* Method::Name() { 
    return mono_method_get_name(this); 
}

void Method::Signature(char* buff) {
    void* pSignature = mono_method_signature(m_base);
    Type* pReturnType = mono_signature_get_return_type(pSignature);
    strcpy_s(buff, 255, mono_type_get_name(pReturnType));
    strcat_s(buff, 255, "(");
    uint32_t nParams = mono_signature_get_param_count(pSignature);
    if (nParams > 0) {
        Type* pType;
        void* iter = 0;
        strcat_s(buff, 255, mono_type_get_name(mono_signature_get_params(pSignature, &iter)));
        while (pType = mono_signature_get_params(pSignature, &iter)) {
            strcat_s(buff, 255, ",");
            strcat_s(buff, 255, mono_type_get_name(pType));
        }
    }
    strcat_s(buff, 255, ")");
}


// Dedinition: Type
System::Type* Type::SystemType() { return mono_type_get_object(mono_get_root_domain(), this); }

#endif