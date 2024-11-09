#include "mono.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include "mono_api.h"
#include "../logger/logger.h"

#ifndef HAX_ASSERT
#include <assert.h>
#define HAX_ASSERT(_EXPR) assert(_EXPR)
#endif

struct MonoThread;
struct MonoDomain;
struct MonoAssembly;
struct MonoObject;
struct MonoType;
struct MonoClass;
struct MonoImage;
struct MonoMethod;
struct MonoString;
struct MonoField;
struct MonoVTable;
struct MonoArray;

#define DO_API(r, n, p) r (*n) p
#include "mono_api.h"
#undef DO_API

static HMODULE GetMonoModule() {
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

void mono::Initialize() {
	HMODULE monoModule = GetMonoModule();
	HAX_ASSERT(monoModule != NULL && "Game not using mono");

	#define DO_API(r, n, p) n = (r (*) p)(GetProcAddress(monoModule, #n))
	#include "mono_api.h"
	#undef DO_API

	LOG_INFO << "mono_get_root_domain address is " << mono_get_root_domain;
	LOG_INFO << "mono_thread_attach address is " << mono_thread_attach;
	LOG_INFO << "mono_domain_get address is " << mono_domain_get;
}