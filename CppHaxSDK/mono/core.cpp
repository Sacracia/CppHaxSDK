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

#define MONO_FUNCTION(a, n, c, m, s) MonoMethodWrapper c ## __ ## m
#define MONO_STATIC_FIELD(a, n, c, f, t) t* c ## __ ## f
#define MONO_FIELD_OFFSET(a, n, c, f)    int c ## __ ## f
#include "core_data.h"
#include "../cheat/game_data.h"
#undef MONO_FUNCTION
#undef MONO_STATIC_FIELD
#undef MONO_FIELD_OFFSET

#ifdef _WIN64
#define UNITY_CORE_ASSEMBLY "UnityEngine.CoreModule"
#else
#define UNITY_CORE_ASSEMBLY "UnityEngine"
#endif

#define ASSERT_GET_PROC_ADDDRESS(f) f = reinterpret_cast<decltype(f)>(GetProcAddress(hMono, #f))
#define HAX_ASSERT(expr, msg) if (!(expr)) {                                                   \
                                 MessageBoxA(NULL, msg, "Hax assertion failed", MB_ICONERROR); \
                                 ExitProcess(-1);                                              \
                              }

static MonoDomain*  g_domain;

MonoDomain*         (*mono_get_root_domain)				    ();
MonoAssembly*       (*mono_domain_assembly_open)		    (MonoDomain* domain, const char* name);
MonoThread*         (*mono_thread_attach)				    (MonoDomain* domain);
MonoDomain*         (*mono_domain_get)					    ();
MonoString*         (*mono_string_new)                      (MonoDomain* domain, const char* string);
MonoImage*          (*mono_assembly_get_image)			    (MonoAssembly* assembly);
MonoClass*          (*mono_class_from_name)				    (MonoImage* image, const char* name_space, const char* name);
MonoMethod*         (*mono_class_get_methods)			    (MonoClass* klass, void** iter);
MonoClassField*     (*mono_class_get_field_from_name)	    (MonoClass* klass, const char* name);
MonoVTable*         (*mono_class_vtable)				    (MonoDomain* domain, MonoClass* klass);
MonoClass*          (*mono_object_get_class)			    (MonoObject* object);
MonoType*           (*mono_class_get_type)                  (MonoClass* klass);
MonoProperty*       (*mono_class_get_property_from_name)    (MonoClass* klass, const char* name);
const char*         (*mono_method_full_name)			    (MonoMethod* method, int32_t signature);
void*               (*mono_compile_method)				    (MonoMethod* method);
MonoJitInfo*        (*mono_jit_info_table_find)			    (MonoDomain* domain, void* addr);
MonoMethod*         (*mono_jit_info_get_method)			    (MonoJitInfo* ji);
MonoObject*         (*mono_runtime_invoke)				    (MonoMethod* method, void* obj, void** params, MonoObject** exc);
void*               (*mono_object_unbox)				    (MonoObject* obj);
void*               (*mono_vtable_get_static_field_data)    (MonoVTable* vt);
MonoReflectionType* (*mono_type_get_object)                 (MonoDomain* domain, MonoType* type);

namespace HaxSdk {
    static HMODULE GetMonoHandle();
    static void InitializeMonoApiFunctions(HMODULE hMono);
    static void InitializeCoreData();
}

namespace HaxSdk {
    void InitializeMono() {
        HMODULE hMono = GetMonoHandle();
        assert(hMono && "Mono not found");
        InitializeMonoApiFunctions(hMono);
        HaxSdk::AttachToThread();
        InitializeCoreData();
    }

    void AttachToThread() {
        mono_get_root_domain()->attach_thread();
        mono_domain_get()->attach_thread();
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
        ASSERT_GET_PROC_ADDDRESS(mono_get_root_domain);
        ASSERT_GET_PROC_ADDDRESS(mono_domain_assembly_open);
        ASSERT_GET_PROC_ADDDRESS(mono_thread_attach);
        ASSERT_GET_PROC_ADDDRESS(mono_domain_get);
        ASSERT_GET_PROC_ADDDRESS(mono_assembly_get_image);
        ASSERT_GET_PROC_ADDDRESS(mono_class_from_name);
        ASSERT_GET_PROC_ADDDRESS(mono_class_get_methods);
        ASSERT_GET_PROC_ADDDRESS(mono_class_get_field_from_name);
        ASSERT_GET_PROC_ADDDRESS(mono_class_vtable);
        ASSERT_GET_PROC_ADDDRESS(mono_object_get_class);
        ASSERT_GET_PROC_ADDDRESS(mono_class_get_type);
        ASSERT_GET_PROC_ADDDRESS(mono_class_get_property_from_name);
        ASSERT_GET_PROC_ADDDRESS(mono_method_full_name);
        ASSERT_GET_PROC_ADDDRESS(mono_compile_method);
        ASSERT_GET_PROC_ADDDRESS(mono_jit_info_table_find);
        ASSERT_GET_PROC_ADDDRESS(mono_jit_info_get_method);
        ASSERT_GET_PROC_ADDDRESS(mono_runtime_invoke);
        ASSERT_GET_PROC_ADDDRESS(mono_object_unbox);
        ASSERT_GET_PROC_ADDDRESS(mono_vtable_get_static_field_data);
        ASSERT_GET_PROC_ADDDRESS(mono_string_new);
        ASSERT_GET_PROC_ADDDRESS(mono_type_get_object);
    }

    static void InitializeCoreData() {
        #define MONO_STATIC_FIELD(a, n, c, f, t)  c ## __ ## f = (t*)MonoClass::find(a, n, #c)->static_field(#f);  \
                                                  std::cout << #c "__" #f ": " << *(c ## __ ## f) << '\n';
        #define MONO_FUNCTION(a, n, c, m, p)  c ## __ ## m = MonoClass::find(a, n, #c)->method(n, #c, #m, p); \
											  std::cout << #c "__" #m ": " << c ## __ ## m ## .ptr << '\n';
        #define MONO_FIELD_OFFSET(a, n, c, f) c ## __ ## f = MonoClass::find(a, n, #c)->field(#f)->offset();\
                                              std::cout << c ## __ ## f << '\n'
        #include "core_data.h"
        #include "../cheat/game_data.h"
        #undef MONO_STATIC_FIELD
        #undef MONO_FUNCTION
        #undef MONO_FIELD_OFFSET
    }
}

#pragma region MonoClasses
MonoMethodWrapper MonoClass::method(const char* signature) {
    void* iter = nullptr;
    MonoMethod* method;
    while (method = mono_class_get_methods(this, &iter)) {
        if (strcmp(signature, mono_method_full_name(method, 1)) == 0) {
            return { method, mono_compile_method(method) };
        }
    }

    HAX_ASSERT(false, std::format("Method {} not found", signature).c_str());
    return { nullptr, nullptr };
}

MonoMethodWrapper MonoClass::method(const char* name_space, const char* klass, const char* name, const char* params) {
	char buffer[255];
	if (name_space != nullptr && name_space[0] != '\0') {
		strcpy_s(buffer, name_space);
		strcat_s(buffer, ".");
		strcat_s(buffer, klass);
	}
	else {
		strcpy_s(buffer, klass);
	}
	strcat_s(buffer, ":");
	strcat_s(buffer, name);
	strcat_s(buffer, " ");
	strcat_s(buffer, params);
	return method(buffer);
}

inline MonoString* MonoString::create(const char* content) {
    return mono_string_new(mono_domain_get(), content);
}

inline void* MonoMethod::ptr() {
    return mono_compile_method(this);
}

inline MonoDomain* MonoDomain::current() {
    return mono_domain_get();
}

inline MonoVTable* MonoDomain::vtable(MonoClass* pClass) {
    return mono_class_vtable(this, pClass);
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

void* MonoClass::static_field(const char* name) {
    MonoVTable* pVTable = mono_class_vtable(mono_domain_get(), this);
    char* pData = (char*)mono_vtable_get_static_field_data(pVTable);
    return (void*)(pData + this->field(name)->offset());
}

MonoClass* MonoClass::find(const char* assembly, const char* name_space, const char* name) {
    return MonoDomain::current()->assembly(assembly)->klass(name_space, name);
}
#pragma endregion

Array<MonoObject*>* Object::FindObjectsOfType(Type* type) {
    return ((Array<MonoObject*>* (*)(Type*))Object__FindObjectsOfType.ptr)(type);
}

MonoObject* Object::FindObjectOfType(Type* type) {
    return ((MonoObject*(*)(Type*))Object__FindObjectOfType.ptr)(type);
}

float Vector3::Distance(Vector3 a, Vector3 b) {
    return ((float(*)(Vector3*, Vector3*))Vector3__Distance.ptr)(&a, &b);
}

Transform* Component::get_transform() {
    return ((Transform*(*)(Component*))Component__get_transform.ptr)(this);
}

Vector3 Transform::get_position() {
    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(Transform__get_position.pMethod, this, nullptr, nullptr));
}

Vector3 Camera::WorldToScreenPoint(Vector3 position) {
    void* args[1] = { &position };
    return *(Vector3*)mono_object_unbox(mono_runtime_invoke(Camera__WorldToScreenPoint.pMethod, this, args, nullptr));
}

Camera* Camera::main() {
    return ((Camera*(*)())Camera__get_main.ptr)();
}

int Screen::height() {
    return ((int(*)())Screen__get_height.ptr)();
}

int Screen::width() {
    return ((int(*)())Screen__get_width.ptr)();
}
