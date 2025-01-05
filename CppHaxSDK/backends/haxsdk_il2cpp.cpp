#ifdef HAX_IL2CPP
#include "haxsdk_il2cpp.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
#include <format>

#include "../haxsdk_gui.h"

#define HAX_ASSERT(expr, msg) if (!(expr)) {                                                   \
                                 MessageBoxA(NULL, msg, "Hax assertion failed", MB_ICONERROR); \
                                 ExitProcess(-1);                                              \
                              }
#define HAX_IL2CPP_PROC(n)    n = reinterpret_cast<t ## _ ## n>(GetProcAddress(hGameAssembly, #n));\
                              HAX_ASSERT(n, #n)

// object
using t_il2cpp_object_new                   = System::Object* (*)(Class* pClass);
using t_il2cpp_object_unbox                 = void* (*)(System::Object* obj);
using t_il2cpp_runtime_object_init          = void (*)(System::Object* obj);

// domain
using t_il2cpp_domain_get                   = Domain* (*)();
using t_il2cpp_domain_get_assemblies        = Assembly** (*)(Domain* domain, size_t * size);

// thread
using t_il2cpp_thread_attach                = Thread* (*)(Domain* domain);

// assembly
using t_il2cpp_assembly_get_image           = Image* (*)(Assembly* assembly);

//class
using t_il2cpp_class_from_name              = Class* (*)(Image* image, const char* namespaze, const char* name);
using t_il2cpp_class_get_type               = Type* (*)(Class* pClass);
using t_il2cpp_class_get_methods            = void* (*)(Class* pClass, void** iter);

// type
using t_il2cpp_type_get_object              = System::Type* (*)(Type* type);
using t_il2cpp_type_get_name                = const char* (*)(Type* type);

// field
using t_il2cpp_class_get_field_from_name    = Field* (*)(Class* pClass, const char* name);

// method
using t_il2cpp_runtime_invoke               = System::Object* (*)(void* pMethod, void* obj, void** params, void** exc);
using t_il2cpp_method_get_param_count       = int32_t (*)(void* pMethod);
using t_il2cpp_method_get_param             = Type* (*)(void* method, uint32_t index);
using t_il2cpp_method_get_class             = Class* (*)(void* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(void* pMethod);
using t_il2cpp_method_get_return_type       = Type* (*)(void* pMethod);

// string
using t_il2cpp_string_new                   = System::String* (*)(const char* str);


// object
static t_il2cpp_object_new                  il2cpp_object_new;
static t_il2cpp_object_unbox                il2cpp_object_unbox;
static t_il2cpp_runtime_object_init         il2cpp_runtime_object_init;

// domain
static t_il2cpp_domain_get                  il2cpp_domain_get;
static t_il2cpp_domain_get_assemblies       il2cpp_domain_get_assemblies;

//thread
static t_il2cpp_thread_attach               il2cpp_thread_attach;

// assembly
static t_il2cpp_assembly_get_image          il2cpp_assembly_get_image;

// class
static t_il2cpp_class_from_name             il2cpp_class_from_name;
static t_il2cpp_class_get_type              il2cpp_class_get_type;
static t_il2cpp_class_get_methods           il2cpp_class_get_methods;

// type
static t_il2cpp_type_get_object             il2cpp_type_get_object;
static t_il2cpp_type_get_name               il2cpp_type_get_name;

// field
static t_il2cpp_class_get_field_from_name   il2cpp_class_get_field_from_name;

// method
static t_il2cpp_runtime_invoke              il2cpp_runtime_invoke;
static t_il2cpp_method_get_param_count      il2cpp_method_get_param_count;
static t_il2cpp_method_get_param            il2cpp_method_get_param;
static t_il2cpp_method_get_class            il2cpp_method_get_class;
static t_il2cpp_method_get_name             il2cpp_method_get_name;
static t_il2cpp_method_get_return_type      il2cpp_method_get_return_type;

// string
static t_il2cpp_string_new                  il2cpp_string_new;


static void     InitializeIl2Cpp(HMODULE hGameAssembly); // Function asserts and finds all IL2CPP api functions 
static HMODULE  GetGameAssemblyHandle();                 // Function finds handle for GameAssembly.dll.

namespace HaxSdk {
    void InitializeBackendData() {
        HMODULE hGameAssembly = GetGameAssemblyHandle();
        HAX_ASSERT(hGameAssembly, "Seems like game doesnt use il2cpp.");
        InitializeIl2Cpp(hGameAssembly);
        Domain::Current()->AttachThread();
    }

    void AttachMenuToUnityThread() {
        Domain::Current()->AttachThread();
    }
}

static void InitializeIl2Cpp(HMODULE hGameAssembly) {
    HAX_IL2CPP_PROC(il2cpp_object_new);
    HAX_IL2CPP_PROC(il2cpp_object_unbox);
    HAX_IL2CPP_PROC(il2cpp_runtime_object_init);
    HAX_IL2CPP_PROC(il2cpp_domain_get);
    HAX_IL2CPP_PROC(il2cpp_domain_get_assemblies);
    HAX_IL2CPP_PROC(il2cpp_thread_attach);
    HAX_IL2CPP_PROC(il2cpp_assembly_get_image);
    HAX_IL2CPP_PROC(il2cpp_class_from_name);
    HAX_IL2CPP_PROC(il2cpp_class_get_type);
    HAX_IL2CPP_PROC(il2cpp_class_get_methods);
    HAX_IL2CPP_PROC(il2cpp_type_get_object);
    HAX_IL2CPP_PROC(il2cpp_type_get_name);
    HAX_IL2CPP_PROC(il2cpp_class_get_field_from_name);
    HAX_IL2CPP_PROC(il2cpp_runtime_invoke);
    HAX_IL2CPP_PROC(il2cpp_method_get_param_count);
    HAX_IL2CPP_PROC(il2cpp_method_get_param);
    HAX_IL2CPP_PROC(il2cpp_method_get_class);
    HAX_IL2CPP_PROC(il2cpp_method_get_name);
    HAX_IL2CPP_PROC(il2cpp_method_get_return_type);
    HAX_IL2CPP_PROC(il2cpp_string_new);
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

System::Object* System::Object::New(Class* pClass) {
    return il2cpp_object_new(pClass);
}

System::Object* System::Object::NewI(Class* pClass) {
    System::Object* pObj = il2cpp_object_new(pClass);
    il2cpp_runtime_object_init(pObj);
    return pObj;
}

void* System::Object::Unbox() {
    return il2cpp_object_unbox(this);
}

System::String* System::String::New(const char* data) {
    return il2cpp_string_new(data);
}

System::String* System::String::Concat(System::String* s1, System::String* s2) {
    static Method pMethod = Class::Find("mscorlib", "System", "String")->FindMethod("Concat", "System.String(System.String,System.String)");
    void* args[] = { s1, s2 };
    return (System::String*)pMethod.Invoke(nullptr, args);
}

std::string System::String::UTF8() {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, this->m_chars, this->m_length, NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, this->m_chars, this->m_length, &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

Domain* Domain::Current() {
    return il2cpp_domain_get();
}

Assembly* Domain::FindAssembly(const char* assembly) {
    Assembly* pAssembly = TryFindAssembly(assembly);
    HAX_ASSERT(pAssembly, std::format("Assembly {} not found", assembly).c_str());
    return pAssembly;
}

Assembly* Domain::TryFindAssembly(const char* assembly) {
    size_t nAssemblies = 0;
    Assembly** ppAssemblies = il2cpp_domain_get_assemblies(Domain::Current(), &nAssemblies);
    for (size_t i = 0; i < nAssemblies; ++i) {
        if (strcmp(ppAssemblies[i]->Name(), assembly) == 0)
            return ppAssemblies[i];
    }
    return nullptr;
}

void Domain::AttachThread() {
    il2cpp_thread_attach(this);
}

Class* Assembly::FindClass(const char* name_space, const char* name) {
    Class* pClass = TryFindClass(name_space, name);
    HAX_ASSERT(pClass, std::format("Class {}.{} not found", name_space, name).c_str());
    return pClass;
}

Class* Assembly::TryFindClass(const char* name_space, const char* name) {
    Image* pImage = il2cpp_assembly_get_image(this);
    Class* pClass = il2cpp_class_from_name(pImage, name_space, name);
    return pClass;
}

Class* Class::Find(const char* assembly, const char* name_space, const char* name) {
    return Domain::FindAssembly(assembly)->FindClass(name_space, name);
}

Class* Class::TryFind(const char* assembly, const char* name_space, const char* name) {
    Assembly* pAssembly = Domain::TryFindAssembly(assembly);
    if (!pAssembly)
        return nullptr;
    return pAssembly->TryFindClass(name_space, name);
}

System::Type* Class::SystemType() {
    Type* pType = il2cpp_class_get_type(this);
    return il2cpp_type_get_object(pType);
}

Method Class::FindMethod(const char* name, const char* sig) {
    void* iter = nullptr;
    void* pMethod;
    char buff[255];
    while (pMethod = il2cpp_class_get_methods(this, &iter)) {
        Method method = (Method::Il2CppMethod*)pMethod;
        if (strcmp(method.Name(), name) == 0) {
            memset(buff, 0, 255);
            method.Signature(buff);
            if (strcmp(buff, sig) == 0)
                return method;
        }
    }
    HAX_ASSERT(false, std::format("Method {} of {} not found in {}", name, sig, this->Name()).c_str());
    return nullptr;
}

Method Class::FindMethod(const char* name) {
    void* iter = nullptr;
    void* pMethod;
    while (pMethod = il2cpp_class_get_methods(this, &iter)) {
        Method method = (Method::Il2CppMethod*)pMethod;
        if (strcmp(method.Name(), name) == 0)
            return method;
    }
    HAX_ASSERT(false, std::format("Method {} not found in {}", name, this->Name()).c_str());
    return nullptr;
}

void* Class::FindStaticField(const char* name) {
    int32_t offset = this->FindField(name)->Offset();
    return (void*)((char*)this->m_staticFields + offset);
}

Field* Class::FindField(const char* name) {
    Field* pField = il2cpp_class_get_field_from_name(this, name);
    HAX_ASSERT(pField, std::format("Field {} not found in {}", name, this->m_name).c_str());
    return pField;
}

System::Object* Method::Invoke(void* obj, void** args) {
    return il2cpp_runtime_invoke(this->m_base, obj, args, nullptr);
}

void Method::Signature(char* buff) {
    strcpy_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_return_type(this->m_base)));
    strcat_s(buff, 255, "(");
    uint32_t nParams = il2cpp_method_get_param_count(this->m_base);
    if (nParams > 0) {
        strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param(this->m_base, 0)));
        for (uint32_t i = 1; i < nParams; ++i) {
            strcat_s(buff, 255, ",");
            strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param(this->m_base, i)));
        }
    }
    strcat_s(buff, 255, ")");
}

Class* Method::Klass() {
    return il2cpp_method_get_class(this->m_base);
}

const char* Method::Name() {
    return il2cpp_method_get_name(this->m_base);
}

#endif