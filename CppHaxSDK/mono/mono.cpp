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

// mono
#define MONO_API_FUNC(r, n, p) r(*n)p
#define MONO_STATIC_FIELD(n, r, c) r c ## _ ## n
#define MONO_FIELD_OFFSET(n, c) int c ## _ ## n
#define MONO_GAME_FUNC(r, n, p, c, s) r(__fastcall *c ## _ ## n)p
#include "mono_api_classes.h"
#include "mono_api_functions.h"
#include "mono_game_data.h"

MonoMethod* MonoObject::GetMonoMethod(void* addr) {
    MonoJitInfo* ji = mono_jit_info_table_find(mono_get_root_domain(), addr);
    return mono_jit_info_get_method(ji);
}

namespace mono {
    MonoDomain* g_domain;
    static std::unordered_map<const char*, MonoClass*> klassCache;

    static HMODULE      GetMonoHandle();
    static void         InitMonoApi(HMODULE hMono);
    static void         InitGameData();
    static MonoClass*   GetMonoClass(const char* assemblyName, const char* name_space, 
                                     const char* klassName);
    static int          GetFieldOffset(const char* assemblyName, const char* name_space,
                                       const char* klassName, const char* fieldName);
    static void*        GetStaticFieldAddress(const char* assemblyName, const char* name_space, 
                                              const char* klassName, const char* fieldName);
    static void*        GetFuncAddress(const char* assemblyName, const char* name_space,
                                       const char* klassName, const char* methodSig);

    void Initialize() {
        HMODULE hMono = GetMonoHandle();
        HAX_ASSERT(hMono != NULL && "Game not using mono");

        InitMonoApi(hMono);

        g_domain = mono_get_root_domain();
        mono_thread_attach(g_domain);
        mono_thread_attach(mono_domain_get());

        InitGameData();
    }

    static void InitGameData() {
        #define MONO_STATIC_FIELD(n, r, c)                                                                                    \
            game::static_fields:: ## c ## _ ## n = (r)(GetStaticFieldAddress(MONO_ASSEMBLY, MONO_NAMESPACE, #c, #n));   \
            std::cout << #c "_" #n << ' ' << game::static_fields:: ## c ## _ ## n << '\n';\
            assert(game::static_fields:: ## c ## _ ## n)
        #define MONO_FIELD_OFFSET(n, c)                                                                                       \
            game::offsets:: ## c ## _ ## n =  GetFieldOffset(MONO_ASSEMBLY, MONO_NAMESPACE, #c, #n);                    \
            std::cout << std::hex << #c "_" #n << ' ' << game::offsets:: ## c ## _ ## n << '\n';\
            assert(game::offsets:: ## c ## _ ## n)
        #define MONO_GAME_FUNC(r, n, p, c, s)                                                                                 \
            game::funcs:: ## c ## _ ## n = (r(__fastcall*)p)GetFuncAddress(MONO_ASSEMBLY, MONO_NAMESPACE, #c, s);                 \
            std::cout << #c "_" #n << ' ' << game::funcs:: ## c ## _ ## n << '\n';\
            assert(game::funcs:: ## c ## _ ## n)
        #include "mono_game_functions.h"
        #include "mono_game_offsets.h"
        #include "mono_game_static_fields.h"
    }

    static void InitMonoApi(HMODULE hMono) {
        #define MONO_API_FUNC(r, n, p)                  \
            n = (r(*)p)(GetProcAddress(hMono, #n));     \
            HAX_ASSERT(n && #n " not found")
        #include "mono_api_functions.h"
        #undef MONO_API_FUNC
    }

    static void* GetStaticFieldAddress(const char* assemblyName, const char* name_space, const char* klassName, const char* fieldName) {
        if (!klassCache.contains(klassName)) {
            klassCache[klassName] = GetMonoClass(assemblyName, name_space, klassName);
        }

        MonoClass* klass = klassCache[klassName];
        MonoVTable* vtable = mono_class_vtable(g_domain, klass);
        MonoClassField* field = mono_class_get_field_from_name(klass, fieldName);
        void* ptr = mono_vtable_get_static_field_data(vtable);
        return (void*)((char*)mono_vtable_get_static_field_data(vtable) + field->offset);
    }

    static int GetFieldOffset(const char* assemblyName, const char* name_space, const char* klassName, const char* fieldName) {
        if (!klassCache.contains(klassName)) {
            klassCache[klassName] = GetMonoClass(assemblyName, name_space, klassName);
        }

        int offset = mono_class_get_field_from_name(klassCache[klassName], fieldName)->offset;
        return offset;
    }

    static void* GetFuncAddress(const char* assemblyName, const char* name_space, const char* klassName, const char* methodSig) {
        if (!klassCache.contains(klassName)) {
            klassCache[klassName] = GetMonoClass(assemblyName, name_space, klassName);
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