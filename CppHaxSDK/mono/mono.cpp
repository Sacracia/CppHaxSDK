#include "mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <sstream>
#include <unordered_map>
#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR) assert(_EXPR)
#endif

#include "../logger/logger.h"
#include "mono_api_classes.h"

#define MONO_API_FUNC(r, n, p) r(*n)p
#include "mono_api_functions.h"
#undef MONO_API_FUNC

MonoMethod* MonoObject::GetMonoMethod(void* addr) {
    MonoJitInfo* ji = mono_jit_info_table_find(mono_get_root_domain(), addr);
    return mono_jit_info_get_method(ji);
}

namespace mono {
    MonoDomain* g_domain;
    static std::unordered_map<const char*, MonoClass*> klassCache;

    static HMODULE      GetMonoHandle();
    static void         InitMonoApi(HMODULE hMono);
    static MonoClass*   GetMonoClass(const char* assemblyName, const char* name_space, const char* klassName);

    void Initialize() {
        HMODULE hMono = GetMonoHandle();
        HAX_ASSERT(hMono != NULL && "Game not using mono");

        InitMonoApi(hMono);

        g_domain = mono_get_root_domain();
        mono_thread_attach(g_domain);
        mono_thread_attach(mono_domain_get());
    }

    void* GetStaticFieldAddress(const char* assemblyName, const char* name_space, const char* klassName, const char* fieldName) {
        if (!klassCache.contains(klassName)) {
            klassCache[klassName] = GetMonoClass(assemblyName, name_space, klassName);
            std::cout << klassName << " is cached\n";
        }

        MonoClass* klass = klassCache[klassName];
        MonoVTable* vtable = mono_class_vtable(g_domain, klass);
        MonoClassField* field = mono_class_get_field_from_name(klass, fieldName);
        void* ptr = mono_vtable_get_static_field_data(vtable);
        return (void*)((char*)mono_vtable_get_static_field_data(vtable) + field->offset);
    }

    int GetFieldOffset(const char* assemblyName, const char* name_space, const char* klassName, const char* fieldName) {
        if (!klassCache.contains(klassName)) {
            klassCache[klassName] = GetMonoClass(assemblyName, name_space, klassName);
            std::cout << klassName << " is cached\n";
        }

        int offset = mono_class_get_field_from_name(klassCache[klassName], fieldName)->offset;
        std::cout << fieldName << " offset is " << std::hex << offset << '\n';
        return offset;
    }

    void* GetFuncAddress(const char* assemblyName, const char* name_space, const char* klassName, const char* methodSig) {
        if (!klassCache.contains(klassName)) {
            klassCache[klassName] = GetMonoClass(assemblyName, name_space, klassName);
            std::cout << klassName << " is cached\n";
        }
        MonoClass* klass = klassCache[klassName];
        char sig[255];
        if (name_space[0] != 0) {
            strcpy(sig, name_space);
            strcat(sig, ".");
            strcat(sig, klassName);
            strcat(sig, ":");
            strcat(sig, methodSig);
        }
        else {
            strcpy(sig, klassName);
            strcat(sig, ":");
            strcat(sig, methodSig);
        }

        void* iter = nullptr;
        MonoMethod* method;
        while (method = mono_class_get_methods(klass, &iter)) {
            if (strcmp(sig,mono_method_full_name(method, 1)) == 0) {
                void* addr = mono_compile_method(method);
                std::cout << methodSig << ' ' << addr << '\n';
                return addr;
            }
        }

        std::cout << sig << ' ' << 0 << '\n';
        return nullptr;
    }

    static MonoClass* GetMonoClass(const char* assemblyName, const char* name_space, const char* klassName) {
        MonoAssembly* assembly = mono_domain_assembly_open(g_domain, assemblyName);
        HAX_ASSERT(assembly != nullptr && "mono_domain_assembly_open failed");
        MonoImage* image = mono_assembly_get_image(assembly);
        HAX_ASSERT(image != nullptr && "mono_assembly_get_image failed");
        MonoClass* klass = mono_class_from_name(image, name_space, klassName);
        HAX_ASSERT(klass != nullptr && "mono_class_from_name failed");
        return klass;
    }

    static void InitMonoApi(HMODULE hMono) {
        #define MONO_API_FUNC(r, n, p)                              \
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
                if (strncmp(me.szModule, "mono", 4) == 0) {
                    CloseHandle(snapshot);
                    return me.hModule;
                }
            } while (Module32Next(snapshot, &me));
        }

        CloseHandle(snapshot);
        return NULL;
    }
}