#include "mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>
#include <sstream>

#include "../logger/logger.h"

#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR) assert(_EXPR)
#endif

#define MONO_API_FUNC(r, n, p) r(*n)p
#include "mono_api_functions.h"
#undef MONO_API_FUNC

#define MONO_GAME_FUNC(r, n, p, s) r(*n)p
#include "mono_game_functions.h"
#undef MONO_GAME_FUNC

MonoDomain* domain;

static HMODULE  GetMonoHandle();
static void     InitMonoApi(HMODULE hMono);
static void     InitGameFunctions();
static void*    GetFuncAddress(std::string signature);
static MonoClass* GetMonoClass(const char* assemblyName, const char* name_space, const char* klassName);

//MonoClass* HeroController::__class = GetMonoClass("Assembly-CSharp", "", "HeroController");
MonoObject* HeroController::playerData() {
    static int offset = -1;
    uintptr_t __this = (uintptr_t)this;
    if (offset < 0) {
        auto __class = GetMonoClass("Assembly-CSharp", "", "HeroController");
        offset = mono_class_get_field_from_name(__class, "playerData")->offset;
        LOG_DEBUG << "Finding offset " << offset << LOG_FLUSH;
    }
    return *(MonoObject**)(__this + offset);
}

void mono::Initialize() {
    HMODULE hMono = GetMonoHandle();
    HAX_ASSERT(hMono != NULL && "Game not using mono");

    InitMonoApi(hMono);

    domain = mono_get_root_domain();
    mono_thread_attach(domain);
    mono_thread_attach(mono_domain_get());

    InitGameFunctions();

    MonoClass* klass = GetMonoClass("Assembly-CSharp", "", "HeroController");
    MonoVTable* vtable = mono_class_vtable(domain, klass);
    MonoClassField* field = mono_class_get_field_from_name(klass, "_instance");
    HAX_ASSERT(field && "Field doesnt exist");

    void* ptr = mono_vtable_get_static_field_data(vtable);
    MonoObject* _instance = *(MonoObject**)((char*)mono_vtable_get_static_field_data(vtable) + field->offset);
    LOG_DEBUG << _instance << LOG_FLUSH;
}

static void InitMonoApi(HMODULE hMono) {
    #define MONO_API_FUNC(r, n, p) n = (r(*)p)(GetProcAddress(hMono, #n));\
						           LOG_DEBUG << #n " address is " << n << LOG_FLUSH;\
                                   HAX_ASSERT(n && #n " not found")
    #include "mono_api_functions.h"
    #undef MONO_API_FUNC
}

static void InitGameFunctions() {
    #define MONO_GAME_FUNC(r, n, p, s) n = (r(*)p)GetFuncAddress(s);\
                                       LOG_DEBUG << #n " address is " << n << LOG_FLUSH;\
                                       HAX_ASSERT(n && #n " not found")
    #include "mono_game_functions.h"
    #undef MONO_GAME_FUNC
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

static void* GetFuncAddress(std::string signature) {
    std::string assemblyName = signature.substr(0, signature.find(", "));
    signature.erase(0, assemblyName.length() + 2);
    std::string sig = signature;
    std::string name_space = "";
    if (auto pos = signature.find("."); pos != std::string::npos) {
        name_space = signature.substr(0, pos);
        signature.erase(0, name_space.length() + 1);
    }
    std::string klassName = signature.substr(0, signature.find(':'));

    MonoAssembly* assembly = mono_domain_assembly_open(domain, assemblyName.c_str());
    HAX_ASSERT(assembly != nullptr && "mono_domain_assembly_open failed");

    MonoImage* image = mono_assembly_get_image(assembly);
    HAX_ASSERT(image != nullptr && "mono_assembly_get_image failed");

    MonoClass* klass = mono_class_from_name(image, name_space.c_str(), klassName.c_str());
    HAX_ASSERT(klass != nullptr && "mono_class_from_name failed");

    void* iter = nullptr;
    MonoMethod* method;
    while (method = mono_class_get_methods(klass, &iter)) {
        if (sig == mono_method_full_name(method, 1)) {
            return mono_compile_method(method);
        }
    }

    return nullptr;
}