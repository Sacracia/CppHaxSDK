#include "core.h"

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

#define MONO_FUNCTION(a, n, c, m, s)     MonoMethod* c ## __ ## m
#define MONO_STATIC_FIELD(a, n, c, f, t) t* c ## __ ## f
#define MONO_FIELD_OFFSET(a, n, c, f)    int c ## __ ## f
#include "core_data.h"
#include "../cheat/game_data.h"
#undef MONO_STATIC_FIELD
#undef MONO_FUNCTION
#undef MONO_FIELD_OFFSET

#ifdef _WIN64
#define UNITY_CORE_ASSEMBLY "UnityEngine.CoreModule"
#else
#define UNITY_CORE_ASSEMBLY "UnityEngine"
#endif

#define HAX_ASSERT(expr, msg) if (!(expr)) {                                                   \
                                 MessageBoxA(NULL, msg, "Hax assertion failed", MB_ICONERROR); \
                                 ExitProcess(-1);                                              \
                              }

using t_mono_get_root_domain                = MonoDomain* (*)();
using t_mono_domain_assembly_open           = MonoAssembly* (*)(MonoDomain* domain, const char* name);
using t_mono_thread_attach                  = MonoThread* (*)(MonoDomain*);
using t_mono_domain_get                     = MonoDomain* (*)();
using t_mono_string_new                     = MonoString* (*)(MonoDomain* domain, const char* string);
using t_mono_assembly_get_image             = MonoImage* (*)(MonoAssembly* assembly);
using t_mono_class_from_name                = MonoClass* (*)(MonoImage* image, const char* name_space, const char* name);
using t_mono_class_get_methods              = MonoMethod* (*)(MonoClass* klass, void** iter);
using t_mono_class_get_field_from_name      = MonoClassField* (*)(MonoClass* klass, const char* name);
using t_mono_class_vtable                   = MonoVTable* (*)(MonoDomain* domain, MonoClass* klass);
using t_mono_object_get_class               = MonoClass* (*)(MonoObject* object);
using t_mono_class_get_type                 = MonoType* (*)(MonoClass* klass);
using t_mono_method_full_name               = const char* (*)(MonoMethod* method, int32_t signature);
using t_mono_compile_method                 = void* (*)(MonoMethod* method);
using t_mono_jit_info_table_find            = MonoJitInfo* (*)(MonoDomain* domain, void* addr);
using t_mono_jit_info_get_method            = MonoMethod* (*)(MonoJitInfo* ji);
using t_mono_runtime_invoke                 = MonoObject* (*)(MonoMethod* method, void* obj, void** params, MonoObject** exc);
using t_mono_object_unbox                   = void* (*)(MonoObject* obj);
using t_mono_vtable_get_static_field_data   = void* (*)(MonoVTable* vt);
using t_mono_type_get_object                = MonoReflectionType* (*)(MonoDomain* domain, MonoType* type);
using t_mono_object_new                     = MonoObject* (*)(MonoDomain* domain, MonoClass* klass);
using t_mono_runtime_object_init            = void (*)(MonoObject* object);
using t_il2cpp_method_get_param_count       = uint32_t (*)(MonoMethod* info);
using t_il2cpp_method_get_param_name        = const char* (*)(MonoMethod* method, uint32_t index);
using t_il2cpp_method_get_name              = const char* (*)(MonoMethod* method);

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
static t_mono_method_full_name             mono_method_full_name;
static t_mono_compile_method               mono_compile_method;
static t_mono_jit_info_table_find          mono_jit_info_table_find;
static t_mono_jit_info_get_method          mono_jit_info_get_method;
static t_mono_runtime_invoke               mono_runtime_invoke;
static t_mono_object_unbox                 mono_object_unbox;
static t_mono_vtable_get_static_field_data mono_vtable_get_static_field_data;
static t_mono_type_get_object              mono_type_get_object;
static t_mono_object_new                   mono_object_new;
static t_mono_runtime_object_init          mono_runtime_object_init;
static t_il2cpp_method_get_param_count     il2cpp_method_get_param_count;
static t_il2cpp_method_get_param_name      il2cpp_method_get_param_name;
static t_il2cpp_method_get_name            il2cpp_method_get_name;

static MonoDomain* g_domain;

namespace HaxSdk {
    static HMODULE GetMonoHandle();
    static void InitializeMonoApiFunctions(HMODULE hMono);
    static void InitializeCoreData();
}

namespace HaxSdk {
    void InitializeCore() {
        HMODULE hMono = GetMonoHandle();
        HAX_ASSERT(hMono, "Unity game not found");
        InitializeMonoApiFunctions(hMono);
        HaxSdk::AttachToThread();
        //InitializeCoreData();

        MonoClass* pClass = MonoClass::find("VampireSurvivors.Runtime", "VampireSurvivors.Framework", "GameManager");
        void* ptr = pClass->method("VampireSurvivors.Framework", "GameManager", "AddFollower", "()")->pointer;
        std::cout << ptr << '\n';
    }

    void AttachToThread() {
        mono_get_root_domain()->attach_thread();
        mono_domain_get()->attach_thread();
    }

    static HMODULE GetMonoHandle() {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
        MODULEENTRY32W me{};
        me.dwSize = sizeof(MODULEENTRY32W);
        if (Module32FirstW(snapshot, &me)) {
            do {
                if (GetProcAddress(me.hModule, "mono_get_root_domain")) {
                    CloseHandle(snapshot);
                    return me.hModule;
                }
            } while (Module32NextW(snapshot, &me));
        }

        CloseHandle(snapshot);
        return NULL;
    }

    static FARPROC GetMonoProcAddress(HMODULE hMono, const char* monoName, const char* il2cppName) {
        FARPROC address = GetProcAddress(hMono, monoName);
        if (!address)
            address = GetProcAddress(hMono, il2cppName);
        HAX_ASSERT(address, monoName);
        return address;
    }

    static void InitializeMonoApiFunctions(HMODULE hMono) {
        mono_get_root_domain = (t_mono_get_root_domain)GetMonoProcAddress(hMono, "mono_get_root_domain", "il2cpp_get_root_domain");
        mono_thread_attach = (t_mono_thread_attach)GetMonoProcAddress(hMono, "mono_thread_attach", "il2cpp_thread_attach");
        mono_object_get_class = (t_mono_object_get_class)GetMonoProcAddress(hMono, "mono_object_get_class", "il2cpp_object_get_class");
        mono_domain_get = (t_mono_domain_get)GetMonoProcAddress(hMono, "mono_domain_get", "il2cpp_domain_get");
        mono_domain_assembly_open = (t_mono_domain_assembly_open)GetMonoProcAddress(hMono, "mono_domain_assembly_open", "il2cpp_domain_assembly_open");
        mono_assembly_get_image = (t_mono_assembly_get_image)GetMonoProcAddress(hMono, "mono_assembly_get_image", "il2cpp_assembly_get_image");
        mono_class_from_name = (t_mono_class_from_name)GetMonoProcAddress(hMono, "mono_class_from_name", "il2cpp_class_from_name");
        mono_class_get_methods = (t_mono_class_get_methods)GetMonoProcAddress(hMono, "mono_class_get_methods", "il2cpp_class_get_methods");
        mono_class_get_type = (t_mono_class_get_type)GetMonoProcAddress(hMono, "mono_class_get_type", "il2cpp_class_get_type");
        mono_class_get_field_from_name = (t_mono_class_get_field_from_name)GetMonoProcAddress(hMono, "mono_class_get_field_from_name", "il2cpp_class_get_field_from_name");
        mono_class_vtable = (t_mono_class_vtable)GetMonoProcAddress(hMono, "mono_class_vtable", "il2cpp_class_vtable");
        mono_method_full_name = (t_mono_method_full_name)GetMonoProcAddress(hMono, "mono_method_full_name", "il2cpp_method_get_full_name");
        mono_compile_method = (t_mono_compile_method)GetProcAddress(hMono, "mono_compile_method");
        mono_jit_info_table_find = (t_mono_jit_info_table_find)GetMonoProcAddress(hMono, "mono_jit_info_table_find", "il2cpp_jit_info_table_find");
        mono_jit_info_get_method = (t_mono_jit_info_get_method)GetMonoProcAddress(hMono, "mono_jit_info_get_method", "il2cpp_jit_info_get_method");
        mono_vtable_get_static_field_data = (t_mono_vtable_get_static_field_data)GetMonoProcAddress(hMono, "mono_vtable_get_static_field_data", "il2cpp_class_get_static_field_data");
        mono_string_new = (t_mono_string_new)GetMonoProcAddress(hMono, "mono_string_new", "il2cpp_string_new");
        mono_object_unbox = (t_mono_object_unbox)GetMonoProcAddress(hMono, "mono_object_unbox", "il2cpp_object_unbox");
        mono_object_new = (t_mono_object_new)GetMonoProcAddress(hMono, "mono_object_new", "il2cpp_object_new");
        mono_runtime_invoke = (t_mono_runtime_invoke)GetMonoProcAddress(hMono, "mono_runtime_invoke", "il2cpp_runtime_invoke");
        mono_runtime_object_init = (t_mono_runtime_object_init)GetMonoProcAddress(hMono, "mono_runtime_object_init", "il2cpp_runtime_object_init");
        il2cpp_method_get_param_count = (t_il2cpp_method_get_param_count)GetProcAddress(hMono, "il2cpp_method_get_param_count");
        il2cpp_method_get_param_name = (t_il2cpp_method_get_param_name)GetProcAddress(hMono, "il2cpp_method_get_param_name");
        il2cpp_method_get_name = (t_il2cpp_method_get_name)GetProcAddress(hMono, "il2cpp_method_get_name");
    }

    static void InitializeCoreData() {
        /*#define MONO_STATIC_FIELD(a, n, c, f, t)  c ## __ ## f = (t*)MonoClass::find(a, n, #c)->static_field(#f);  \
                                                  std::cout << #c "__" #f ": " << *(c ## __ ## f) << '\n';
        #define MONO_FUNCTION(a, n, c, m, p)  c ## __ ## m = MonoClass::find(a, n, #c)->method(n, #c, #m, p); \
											  std::cout << #c "__" #m ": " << c ## __ ## m ## .ptr << '\n';
        #define MONO_FIELD_OFFSET(a, n, c, f) c ## __ ## f = MonoClass::find(a, n, #c)->field(#f)->offset;\
                                              std::cout << c ## __ ## f << '\n'
        #include "core_data.h"
        #include "../cheat/game_data.h"*/
    }
}

#pragma region MonoClasses
#ifdef HAX_MONO
MonoMethod* MonoClass::method(const char* signature) {
    void* iter = nullptr;
    MonoMethod* method;
    while (method = mono_class_get_methods(this, &iter)) {
        if (strcmp(signature, mono_method_full_name(method, 1)) == 0) {
            return method;
        }
    }

    HAX_ASSERT(false, std::format("Method {} not found", signature).c_str());
    return nullptr;
}

MonoMethod* MonoClass::method(const char* name_space, const char* klass, const char* name, const char* params) {
	char buffer[255];
	if (name_space != nullptr && name_space[0] != '\0') {
		strcpy_s(buffer, name_space);
		strcat_s(buffer, ".");
		strcat_s(buffer, klass);
	}
	else {
		strcpy_s(buffer, klass);
	}
    if (GetModuleHandle("GameAssembly.dll"))
        strcat_s(buffer, ":");
	strcat_s(buffer, ":");
	strcat_s(buffer, name);
	strcat_s(buffer, " ");
	strcat_s(buffer, params);
	return method(buffer);
}
#else
MonoMethod* MonoClass::method(const char* signature) {
    HAX_ASSERT(false, "method not implemented");
    return nullptr;
}

MonoMethod* MonoClass::method(const char* name_space, const char* klass, const char* name, const char* params) {
    void* iter = nullptr;
    MonoMethod* method;
    while (method = mono_class_get_methods(this, &iter)) {
        if (strcmp(name, il2cpp_method_get_name(method)) == 0) {
            char buff[255] = "(";
            uint32_t nParams = il2cpp_method_get_param_count(method);
            if (nParams > 0) {
                strcat_s(buff, il2cpp_method_get_param_name(method, 0));
                for (uint32_t i = 1; i < il2cpp_method_get_param_count(method); ++i) {
                    strcat_s(buff, ",");
                    strcat_s(buff, il2cpp_method_get_param_name(method, i));
                }
            }
            strcat_s(buff, ")");
            std::cout << buff << '\n';
            if (strcmp(buff, params) == 0)
                return method;
        }
    }

    HAX_ASSERT(false, std::format("Method {} not found", name).c_str());
    return nullptr;
}
#endif

inline MonoString* MonoString::create(const char* content) {
    return mono_string_new(mono_domain_get(), content);
}

#ifdef HAX_MONO
inline void* MonoMethod::ptr() {
    return mono_compile_method(this);
}
#endif

inline MonoDomain* MonoDomain::current() {
    return mono_domain_get();
}

MonoAssembly* MonoDomain::assembly(const char* assembly) {
    auto pAssembly = mono_domain_assembly_open(this, assembly);
    HAX_ASSERT(pAssembly, std::format("Assembly {} not found", assembly).c_str());
    return pAssembly;
}

inline void MonoDomain::attach_thread() {
    mono_thread_attach(this);
}

MonoClass* MonoAssembly::klass(const char* name_space, const char* name) {
    auto pClass = mono_class_from_name(mono_assembly_get_image(this), name_space, name);
    HAX_ASSERT(pClass, std::format("Class {}.{} not found", name_space, name).c_str());
    return pClass;
}

MonoReflectionType* MonoClass::system_type() {
    return mono_type_get_object(mono_domain_get(), mono_class_get_type(this));
}

MonoClassField* MonoClass::field(const char* name) {
    auto pField = mono_class_get_field_from_name(this, name);
    HAX_ASSERT(pField, std::format("Field {} not found", name).c_str());
    return pField;
}

MonoObject* MonoClass::ctor() {
    MonoObject* obj = mono_object_new(MonoDomain::current(), this);
    mono_runtime_object_init(obj);
    return obj;
}

void* MonoClass::static_field(const char* name) {
#ifdef HAX_MONO
    MonoVTable* pVTable = mono_class_vtable(mono_domain_get(), this);
    char* pData = (char*)mono_vtable_get_static_field_data(pVTable);
#else
    char* pData = *(char**)this->static_fields;
#endif
    return (void*)(pData + this->field(name)->offset);
}

MonoClass* MonoClass::find(const char* assembly, const char* name_space, const char* name) {
    return MonoDomain::current()->assembly(assembly)->klass(name_space, name);
}
#pragma endregion

//Array<MonoObject*>* Object::FindObjectsOfType(Type* type) {
//    return ((Array<MonoObject*>* (*)(Type*))Object__FindObjectsOfType.ptr)(type);
//}
//
//MonoObject* Object::FindObjectOfType(Type* type) {
//    return ((MonoObject*(*)(Type*))Object__FindObjectOfType.ptr)(type);
//}
//
//float Vector3::Distance(Vector3 a, Vector3 b) {
//    return ((float(*)(Vector3*, Vector3*))Vector3__Distance.ptr)(&a, &b);
//}
//
//Transform* Component::get_transform() {
//    return ((Transform*(*)(Component*))Component__get_transform.ptr)(this);
//}
//
//Vector3 Transform::get_position() {
//    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(Transform__get_position.pMethod, this, nullptr, nullptr));
//}
//
//void Transform::set_position(Vector3 value) {
//    ((void(*)(Vector3*))Transform__set_position.ptr)(&value);
//}
//
//Vector3 Camera::WorldToScreenPoint(Vector3 position) {
//    void* args[1] = { &position };
//    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(Camera__WorldToScreenPoint.pMethod, this, args, nullptr));
//}
//
//Camera* Camera::main() {
//    return ((Camera*(*)())Camera__get_main.ptr)();
//}
//
//int Screen::height() {
//    return ((int(*)())Screen__get_height.ptr)();
//}
//
//int Screen::width() {
//    return ((int(*)())Screen__get_width.ptr)();
//}
