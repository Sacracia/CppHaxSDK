// [SECTION] INCLUDES
// [SECTION] FORWARD DECLARATIONS
// [SECTION] 
// [SECTION] USER FACING STRUCTURES (ImGuiStyle, ImGuiIO)
// [SECTION] MISC HELPERS/UTILITIES (Geometry functions)
// [SECTION] MISC HELPERS/UTILITIES (String, Format, Hash functions)
// [SECTION] MISC HELPERS/UTILITIES (File functions)
// [SECTION] MISC HELPERS/UTILITIES (ImText* functions)
// [SECTION] MISC HELPERS/UTILITIES (Color functions)
// [SECTION] ImGuiStorage
// [SECTION] ImGuiTextFilter
// [SECTION] ImGuiTextBuffer, ImGuiTextIndex
// [SECTION] ImGuiListClipper
// [SECTION] STYLING
// [SECTION] RENDER HELPERS
// [SECTION] INITIALIZATION, SHUTDOWN
// [SECTION] MAIN CODE (most of the code! lots of stuff, needs tidying up!)
// [SECTION] ID STACK
// [SECTION] INPUTS
// [SECTION] ERROR CHECKING
// [SECTION] ITEM SUBMISSION
// [SECTION] LAYOUT
// [SECTION] SCROLLING
// [SECTION] TOOLTIPS
// [SECTION] POPUPS
// [SECTION] KEYBOARD/GAMEPAD NAVIGATION
// [SECTION] DRAG AND DROP
// [SECTION] LOGGING/CAPTURING
// [SECTION] SETTINGS
// [SECTION] LOCALIZATION
// [SECTION] VIEWPORTS, PLATFORM WINDOWS
// [SECTION] PLATFORM DEPENDENT HELPERS
// [SECTION] METRICS/DEBUGGER WINDOW
// [SECTION] DEBUG LOG WINDOW
// [SECTION] OTHER DEBUG TOOLS (ITEM PICKER, ID STACK TOOL)


//-----------------------------------------------------------------------------
// [SECTION] INCLUDES
//-----------------------------------------------------------------------------

#include "haxsdk.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <iostream>
#include <format>

#include "logger/logger.h"

#define BACKEND_API_DECL(_RET, _FUNC, _PARAMS) using t_ ## _FUNC = _RET (*)_PARAMS; static t_ ## _FUNC _FUNC = nullptr
#define BACKEND_API_DEF(_FUNC, _MODULE) _FUNC = (t_ ## _FUNC)GetProcAddress(_MODULE, #_FUNC); HAX_ASSERT(_FUNC, #_FUNC); std::cout << #_FUNC << ' ' << std::hex << _FUNC << '\n'
#define BACKEND_API_DEF_OPT(_FUNC, _MODULE) _FUNC = (t_ ## _FUNC)GetProcAddress(_MODULE, #_FUNC); std::cout << #_FUNC << ' ' << std::hex << _FUNC << '\n'

static HaxGlobals g_globals;

HaxGlobals& HaxSdk::GetGlobals() {
    return g_globals;
}

struct Il2CppMethod;
struct MonoMethod;

using MethodSignature = void;
using t_mono_get_root_domain                = Domain* (*)();
using t_mono_domain_assembly_open           = Assembly* (*)(Domain*, const char*);
using t_mono_thread_attach                  = Thread* (*)(Domain*);
using t_mono_domain_get                     = Domain* (*)();
using t_mono_string_new                     = System::String* (*)(Domain*, const char*);
using t_mono_assembly_get_image             = Image* (*)(Assembly*);
using t_mono_class_from_name                = Class* (*)(Image*, const char*, const char*);
using t_mono_class_get_methods              = Method* (*)(Class*, void**);
using t_mono_class_get_field_from_name      = Field* (*)(Class*, const char*);
using t_mono_class_vtable                   = void* (*)(Domain*, Class*);
using t_mono_class_get_namespace            = const char* (*)(Class*);
using t_mono_class_get_name                 = const char* (*)(Class*);
using t_mono_object_get_class               = Class* (*)(System::Object*);
using t_mono_class_get_type                 = Type* (*)(Class*);
using t_mono_object_get_class               = Class* (*)(System::Object*);
using t_mono_method_signature               = MethodSignature* (*)(void*);
using t_mono_method_get_name                = const char* (*)(void*);
using t_mono_compile_method                 = void* (*)(void*);
using t_mono_jit_info_table_find            = void* (*)(Domain*, void*);
using t_mono_jit_info_get_method            = Method* (*)(void*);
using t_mono_runtime_invoke                 = System::Object* (*)(void*, void*, void**, System::Object**);
using t_mono_print_unhandled_exception      = void (*)(System::Object*);
using t_mono_object_unbox                   = void* (*)(System::Object*);
using t_mono_vtable_get_static_field_data   = void* (*)(void*);
using t_mono_type_get_object                = System::Type* (*)(Domain*, Type*);
using t_mono_object_new                     = System::Object* (*)(Domain*, Class*);
using t_mono_runtime_object_init            = void (*)(System::Object*);
using t_mono_signature_get_return_type      = Type* (*)(MethodSignature*);
using t_mono_type_get_name                  = char* (*)(Type*);
using t_mono_signature_get_param_count      = uint32_t (*)(MethodSignature*);
using t_mono_signature_get_params           = Type* (*)(MethodSignature*, void**);
using t_mono_method_get_unmanaged_thunk     = void* (*)(void*);
using t_mono_string_to_utf8                 = char* (*)(System::String*);
using t_mono_field_static_get_value         = void (*)(void* pVtable, Field* pField, void* pValue);
using t_mono_field_static_set_value         = void (*)(void* pVtable, Field* pField, void* pValue);

static t_mono_get_root_domain               mono_get_root_domain;
static t_mono_domain_assembly_open          mono_domain_assembly_open;
static t_mono_thread_attach                 mono_thread_attach;
static t_mono_domain_get                    mono_domain_get;
static t_mono_string_new                    mono_string_new;
static t_mono_assembly_get_image            mono_assembly_get_image;
static t_mono_class_from_name               mono_class_from_name;
static t_mono_class_get_methods             mono_class_get_methods;
static t_mono_class_get_field_from_name     mono_class_get_field_from_name;
static t_mono_class_get_namespace           mono_class_get_namespace;
static t_mono_class_get_name                mono_class_get_name;
static t_mono_class_vtable                  mono_class_vtable;
static t_mono_object_get_class              mono_object_get_class;
static t_mono_class_get_type                mono_class_get_type;
static t_mono_method_signature              mono_method_signature;
static t_mono_method_get_name               mono_method_get_name;
static t_mono_compile_method                mono_compile_method;
static t_mono_jit_info_table_find           mono_jit_info_table_find;
static t_mono_jit_info_get_method           mono_jit_info_get_method;
static t_mono_runtime_invoke                mono_runtime_invoke;
static t_mono_object_unbox                  mono_object_unbox;
static t_mono_vtable_get_static_field_data  mono_vtable_get_static_field_data;
static t_mono_type_get_object               mono_type_get_object;
static t_mono_object_new                    mono_object_new;
static t_mono_runtime_object_init           mono_runtime_object_init;
static t_mono_signature_get_return_type     mono_signature_get_return_type;
static t_mono_type_get_name                 mono_type_get_name;
static t_mono_signature_get_param_count     mono_signature_get_param_count;
static t_mono_signature_get_params          mono_signature_get_params;
static t_mono_method_get_unmanaged_thunk    mono_method_get_unmanaged_thunk;
static t_mono_string_to_utf8                mono_string_to_utf8;
static t_mono_field_static_get_value        mono_field_static_get_value;
static t_mono_field_static_set_value        mono_field_static_set_value;

using t_il2cpp_object_new                   = System::Object* (*)(Class* pClass);
using t_il2cpp_object_unbox                 = void* (*)(System::Object* obj);
using t_il2cpp_runtime_object_init          = void (*)(System::Object* obj);
using t_il2cpp_domain_get                   = Domain* (*)();
using t_il2cpp_domain_get_assemblies        = Assembly * *(*)(Domain* domain, size_t* size);
using t_il2cpp_thread_attach                = Thread * (*)(Domain* domain);
using t_il2cpp_assembly_get_image           = Image * (*)(Assembly* assembly);
using t_il2cpp_class_from_name              = Class * (*)(Image* image, const char* namespaze, const char* name);
using t_il2cpp_class_get_type               = Type * (*)(Class* pClass);
using t_il2cpp_class_get_methods            = Method* (*)(Class* pClass, void** ppIter);
using t_il2cpp_type_get_object              = System::Type* (*)(Type* type);
using t_il2cpp_type_get_name                = const char* (*)(Type* type);
using t_il2cpp_class_get_field_from_name    = Field * (*)(Class* pClass, const char* name);
using t_il2cpp_runtime_invoke               = System::Object* (*)(Method* pMethod, void* obj, void** params, void** exc);
using t_il2cpp_method_get_param_count       = int32_t(*)(Method* pMethod);
using t_il2cpp_method_get_param             = Type * (*)(Method* pMethod, uint32_t index);
using t_il2cpp_method_get_class             = Class * (*)(Method* pMethod);
using t_il2cpp_method_get_name              = const char* (*)(Method* pMethod);
using t_il2cpp_method_get_return_type       = Type * (*)(Method* pMethod);
using t_il2cpp_string_new                   = System::String* (*)(const char* str);
using t_il2cpp_class_get_static_field_data  = void* (*)(Class*);
using t_il2cpp_field_static_get_value       = void (*)(Field* pField, void* pValue);
using t_il2cpp_field_static_set_value       = void (*)(Field* pField, void* pValue);

static t_il2cpp_object_new                  il2cpp_object_new;
static t_il2cpp_object_unbox                il2cpp_object_unbox;
static t_il2cpp_runtime_object_init         il2cpp_runtime_object_init;
static t_il2cpp_domain_get                  il2cpp_domain_get;
static t_il2cpp_domain_get_assemblies       il2cpp_domain_get_assemblies;
static t_il2cpp_thread_attach               il2cpp_thread_attach;
static t_il2cpp_assembly_get_image          il2cpp_assembly_get_image;
static t_il2cpp_class_from_name             il2cpp_class_from_name;
static t_il2cpp_class_get_type              il2cpp_class_get_type;
static t_il2cpp_class_get_methods           il2cpp_class_get_methods;
static t_il2cpp_type_get_object             il2cpp_type_get_object;
static t_il2cpp_type_get_name               il2cpp_type_get_name;
static t_il2cpp_class_get_field_from_name   il2cpp_class_get_field_from_name;
static t_il2cpp_runtime_invoke              il2cpp_runtime_invoke;
static t_il2cpp_method_get_param_count      il2cpp_method_get_param_count;
static t_il2cpp_method_get_param            il2cpp_method_get_param;
static t_il2cpp_method_get_class            il2cpp_method_get_class;
static t_il2cpp_method_get_name             il2cpp_method_get_name;
static t_il2cpp_method_get_return_type      il2cpp_method_get_return_type;
static t_il2cpp_string_new                  il2cpp_string_new;
static t_il2cpp_class_get_static_field_data il2cpp_class_get_static_field_data;
static t_il2cpp_field_static_get_value      il2cpp_field_static_get_value;
static t_il2cpp_field_static_set_value      il2cpp_field_static_set_value;

static void DetermineBackend();
static void DefineBackendApi();
static void GetSignature(MonoMethod* pMethod, char* buff);
static void GetSignature(Il2CppMethod* pMethod, char* buff);

void HaxSdk::InitializeCore() {
    DetermineBackend();
    HAX_ASSERT(g_globals.backend != HaxBackend_None, "Unable to determine unity backend");
    DefineBackendApi();
    UnityAttachThread();
}

void HaxSdk::UnityAttachThread() {
    Domain::Main()->AttachThread();
}

static void DetermineBackend() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
    MODULEENTRY32W moduleEntry = {0};
    moduleEntry.dwSize = sizeof(moduleEntry);
    if (Module32FirstW(hSnapshot, &moduleEntry)) {
        do {
            if (wcsncmp(moduleEntry.szModule, L"mono", 4) == 0) {
                CloseHandle(hSnapshot);
                g_globals.backend = HaxBackend_Mono;
                g_globals.backendHandle = (void*)moduleEntry.hModule;
                return;
            }
            if (wcscmp(moduleEntry.szModule, L"GameAssembly.dll") == 0) {
                CloseHandle(hSnapshot);
                g_globals.backend = HaxBackend_IL2CPP;
                g_globals.backendHandle = (void*)moduleEntry.hModule;
                return;
            }
        } while (Module32NextW(hSnapshot, &moduleEntry));
    }
    CloseHandle(hSnapshot);
    g_globals.backend = HaxBackend_None;
}

static void DefineBackendApi() {
    HMODULE hModule = (HMODULE)g_globals.backendHandle;
    if (g_globals.backend & HaxBackend_IL2CPP) {
        BACKEND_API_DEF(il2cpp_object_new, hModule);
        BACKEND_API_DEF(il2cpp_object_unbox, hModule);
        BACKEND_API_DEF(il2cpp_runtime_object_init, hModule);
        BACKEND_API_DEF(il2cpp_domain_get, hModule);
        BACKEND_API_DEF(il2cpp_domain_get_assemblies, hModule);
        BACKEND_API_DEF(il2cpp_thread_attach, hModule);
        BACKEND_API_DEF(il2cpp_assembly_get_image, hModule);
        BACKEND_API_DEF(il2cpp_class_from_name, hModule);
        BACKEND_API_DEF(il2cpp_class_get_type, hModule);
        BACKEND_API_DEF(il2cpp_class_get_methods, hModule);
        BACKEND_API_DEF(il2cpp_type_get_object, hModule);
        BACKEND_API_DEF(il2cpp_type_get_name, hModule);
        BACKEND_API_DEF(il2cpp_class_get_field_from_name, hModule);
        BACKEND_API_DEF(il2cpp_runtime_invoke, hModule);
        BACKEND_API_DEF(il2cpp_method_get_param_count, hModule);
        BACKEND_API_DEF(il2cpp_method_get_param, hModule);
        BACKEND_API_DEF(il2cpp_method_get_class, hModule);
        BACKEND_API_DEF(il2cpp_method_get_name, hModule);
        BACKEND_API_DEF(il2cpp_method_get_return_type, hModule);
        BACKEND_API_DEF(il2cpp_string_new, hModule);
        BACKEND_API_DEF(il2cpp_field_static_get_value, hModule);
        BACKEND_API_DEF(il2cpp_field_static_set_value, hModule);
        BACKEND_API_DEF_OPT(il2cpp_class_get_static_field_data, hModule);
    }
    else {
        BACKEND_API_DEF(mono_get_root_domain, hModule);
        BACKEND_API_DEF(mono_domain_assembly_open, hModule);
        BACKEND_API_DEF(mono_thread_attach, hModule);
        BACKEND_API_DEF(mono_domain_get, hModule);
        BACKEND_API_DEF(mono_string_new, hModule);
        BACKEND_API_DEF(mono_assembly_get_image, hModule);
        BACKEND_API_DEF(mono_class_from_name, hModule);
        BACKEND_API_DEF(mono_class_get_methods, hModule);
        BACKEND_API_DEF(mono_class_get_field_from_name, hModule);
        BACKEND_API_DEF(mono_class_vtable, hModule);
        BACKEND_API_DEF(mono_class_get_name, hModule);
        BACKEND_API_DEF(mono_class_get_namespace, hModule);
        BACKEND_API_DEF(mono_object_get_class, hModule);
        BACKEND_API_DEF(mono_class_get_type, hModule);
        BACKEND_API_DEF(mono_method_signature, hModule);
        BACKEND_API_DEF(mono_method_get_name, hModule);
        BACKEND_API_DEF(mono_compile_method, hModule);
        BACKEND_API_DEF(mono_jit_info_table_find, hModule);
        BACKEND_API_DEF(mono_jit_info_get_method, hModule);
        BACKEND_API_DEF(mono_runtime_invoke, hModule);
        BACKEND_API_DEF(mono_object_unbox, hModule);
        BACKEND_API_DEF(mono_vtable_get_static_field_data, hModule);
        BACKEND_API_DEF(mono_type_get_object, hModule);
        BACKEND_API_DEF(mono_object_new, hModule);
        BACKEND_API_DEF(mono_runtime_object_init, hModule);
        BACKEND_API_DEF(mono_signature_get_return_type, hModule);
        BACKEND_API_DEF(mono_type_get_name, hModule);
        BACKEND_API_DEF(mono_signature_get_param_count, hModule);
        BACKEND_API_DEF(mono_signature_get_params, hModule);
        BACKEND_API_DEF(mono_string_to_utf8, hModule);
        BACKEND_API_DEF(mono_field_static_get_value, hModule);
        BACKEND_API_DEF(mono_field_static_set_value, hModule);
    }
}

struct Il2CppAssembly {
    Image*      pImage;
    uint32_t    token;
    int32_t     referencedAssemblyStart;
    int32_t     referencedAssemblyCount;
    const char* name;
};

struct MonoAssembly {
};

Assembly* Assembly::Find(const char* name) {
    Assembly* pAssembly;
    if (Exists(name, pAssembly))
        return pAssembly;

    HaxSdk::Log(std::format("Assembly {} not found", name));
    HAX_ASSERT(false, "Assembly not found.\nSee logs for more information");
    return pAssembly;
}

bool Assembly::Exists(const char* name, OUT Assembly*& pRes) {
    Domain* pDomain = Domain::Main();

    if (g_globals.backend & HaxBackend_Mono) {
        pRes = mono_domain_assembly_open(pDomain, name);
        return pRes != nullptr;
    }

    size_t nDomains = 0;
    Assembly** ppAssembly = il2cpp_domain_get_assemblies(pDomain, &nDomains);
    for (size_t i = 0; i < nDomains; ++i) {
        if (strcmp(((Il2CppAssembly*)ppAssembly[i])->name, name) == 0) {
            pRes = ppAssembly[i];
            return true;
        }
    }

    return false;
}

Image* Assembly::GetImage() {
    return g_globals.backend & HaxBackend_IL2CPP ? ((Il2CppAssembly*)this)->pImage : mono_assembly_get_image(this);
}

struct Il2CppClass {
    Image*                  image;
    void*                   gcDesc;
    const char*             name;
    const char*             nameSpace;
    Type                    byvalArg;
    Type                    thisArg;
    void*                   __space[15];
    void*                   staticFields;
};

bool Class::Exists(const char* assembly, const char* nameSpace, const char* name, OUT Class*& pClass) {
    Assembly* pAssembly;
    if (!Assembly::Exists(assembly, pAssembly)) {
        pClass = nullptr;
        return false;
    }

    pClass = g_globals.backend & HaxBackend_IL2CPP ? il2cpp_class_from_name(pAssembly->GetImage(), nameSpace, name) : 
        mono_class_from_name(pAssembly->GetImage(), nameSpace, name);
    return pClass != nullptr;
}

Class* Class::Find(const char* assembly, const char* nameSpace, const char* name) {
    Class* pClass;
    if (Exists(assembly, nameSpace, name, pClass))
        return pClass;
    
    HaxSdk::Log(std::format("Class {}.{} not found in {}", nameSpace, name, assembly));
    HAX_ASSERT(false, "Class not found.\nSee logs for more information");
    return pClass;
}

Method* Class::FindMethod(const char* name, const char* signature) {
    bool isMono = HaxSdk::GetGlobals().backend & HaxBackend_Mono;
    void* iter = nullptr;

    while (Method* pMethod = isMono ? mono_class_get_methods(this, &iter) : il2cpp_class_get_methods(this, &iter)) {
        const char* methodName = isMono ? mono_method_get_name(pMethod) : il2cpp_method_get_name(pMethod);
        if (strcmp(methodName, name) == 0) {
            if (!signature || signature[0] == '\0') {
                std::cout << name << ' ' << std::hex << pMethod->GetAddress() << '\n';
                return pMethod;
            }

            char buff[256] = {0};
            isMono ? GetSignature((MonoMethod*)pMethod, buff) : GetSignature((Il2CppMethod*)pMethod, buff);
            if (strcmp(signature, buff) == 0) {
                std::cout << name << signature << ' ' << std::hex << pMethod->GetAddress() << '\n';
                return pMethod;
            }
        }
    }

    HaxSdk::Log(std::format("Method {} of {} not found in {}", name, signature ? signature : "?", this->GetName()));
    HAX_ASSERT(false, "Method not found.\nSee logs for more information");
    return nullptr;
}

struct Il2CppField {
    const char* name;
    Type*       type;
    Class*      parent;
    int32_t     offset;
    uint32_t    token;
};

struct MonoField {
    Type*       type;
    const char* name;
    Class*      parent;
    int32_t     offset;
};

Field* Class::FindField(const char* name) {
    Field* pField = g_globals.backend & HaxBackend_Mono ? mono_class_get_field_from_name(this, name)
        : il2cpp_class_get_field_from_name(this, name);
    if (!pField) {
        HaxSdk::Log(std::format("Field {} not found in {}", name, this->GetName()));
        HAX_ASSERT(false, "Field not found.\nSee logs for more information");
    }
    return pField;
}

void* Class::FindStaticField(const char* name) {
    int32_t offset = FindField(name)->Offset();
    return (void*)((char*)GetStaticFieldsData() + offset);
}

void* Class::GetStaticFieldsData() {
    if (g_globals.backend & HaxBackend_IL2CPP)
        return il2cpp_class_get_static_field_data ? il2cpp_class_get_static_field_data(this) : ((Il2CppClass*)this)->staticFields;
    return mono_vtable_get_static_field_data(mono_class_vtable(Domain::Main(), this));
}

int32_t Field::Offset() {
    return g_globals.backend & HaxBackend_IL2CPP ? ((Il2CppField*)this)->offset : ((MonoField*)this)->offset;
}

void Field::GetStaticValue(void* pValue) {
    if (g_globals.backend & HaxBackend_Mono) {
        void* pVtable = mono_class_vtable(Domain::Main(), ((MonoField*)this)->parent);
        mono_field_static_get_value(pVtable, this, pValue);
        return;
    }
    il2cpp_field_static_get_value(this, pValue);
}

void Field::SetStaticValue(void* pValue) {
    if (g_globals.backend & HaxBackend_Mono) {
        void* pVtable = mono_class_vtable(Domain::Main(), ((MonoField*)this)->parent);
        mono_field_static_set_value(pVtable, this, pValue);
        return;
    }
    il2cpp_field_static_set_value(this, pValue);
}

const char* Class::GetName() {
    return g_globals.backend & HaxBackend_Mono ? mono_class_get_name(this) : ((Il2CppClass*)this)->name;
}

System::Type* Class::GetSystemType() {
    return g_globals.backend & HaxBackend_Mono ? mono_class_get_type(this)->GetSystemType() : il2cpp_class_get_type(this)->GetSystemType();
}

Domain* Domain::Main() {
    return g_globals.backend & HaxBackend_Mono ? mono_get_root_domain() : il2cpp_domain_get();
}

void Domain::AttachThread() {
    Domain* pDomain = Domain::Main();
    g_globals.backend & HaxBackend_Mono ? mono_thread_attach(pDomain) : il2cpp_thread_attach(pDomain);
}

struct Il2CppType {
    void* m_data;
    unsigned int            m_attrs : 16;
    int                     m_type : 8;
    unsigned int            m_num_mods : 5;
    unsigned int            m_byref : 1;
    unsigned int            m_pinned : 1;
    unsigned int            m_valuetype : 1;
};

System::Type* Type::GetSystemType() {
    return g_globals.backend & HaxBackend_Mono ? mono_type_get_object(Domain::Main(), this) : il2cpp_type_get_object(this);
}

System::Object* System::Object::New(Class* pClass) {
    return g_globals.backend & HaxBackend_Mono ? mono_object_new(Domain::Main(), pClass) : il2cpp_object_new(pClass);
}

System::Object* System::Object::Ctor() {
    g_globals.backend& HaxBackend_Mono ? mono_runtime_object_init(this) : il2cpp_runtime_object_init(this);
    return this;
}

void* System::Object::Unbox() {
    return g_globals.backend & HaxBackend_Mono ? mono_object_unbox(this) : il2cpp_object_unbox(this);
}

struct Il2CppMethod {
    void* ptr;
    // <...>
};

System::Object* Method::Invoke(void* __this, void** ppArgs) {
    return g_globals.backend & HaxBackend_Mono ? mono_runtime_invoke(this, __this, ppArgs, nullptr) : il2cpp_runtime_invoke(this, __this, ppArgs, nullptr);
}

void* Method::GetAddress() {
    return g_globals.backend & HaxBackend_Mono ? mono_compile_method(this) : ((Il2CppMethod*)this)->ptr;
}

System::String* System::String::New(const char* data) {
    return HaxSdk::GetGlobals().backend & HaxBackend_Mono ? mono_string_new(Domain::Main(), data) : il2cpp_string_new(data);
}

void GetSignature(MonoMethod* pMethod, char* buff) {
    void* pSignature = mono_method_signature(pMethod);
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

void GetSignature(Il2CppMethod* pMethod, char* buff) {
    strcpy_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_return_type((Method*)pMethod)));
    strcat_s(buff, 255, "(");
    uint32_t nParams = il2cpp_method_get_param_count((Method*)pMethod);
    if (nParams > 0) {
        strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param((Method*)pMethod, 0)));
        for (uint32_t i = 1; i < nParams; ++i) {
            strcat_s(buff, 255, ",");
            strcat_s(buff, 255, il2cpp_type_get_name(il2cpp_method_get_param((Method*)pMethod, i)));
        }
    }
    strcat_s(buff, 255, ")");
}