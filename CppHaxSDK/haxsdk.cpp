#include "haxsdk.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include "backend/haxsdk_mono.h"

static HaxGlobals g_globals;

static bool GetBackend(HaxBackend& backend, void*& backendBaseAddr);

HaxGlobals& GetGlobals()
{
	return g_globals;
}

void HaxSdk::Initialize()
{
	bool status = GetBackend(g_globals.backend, g_globals.backendHandle);
	HAX_ASSERT(status, "Unable to determine backend. Game is not Unity");

	g_globals.backend == HaxBackend_Mono ? Mono::Initialize();
}

static bool GetBackend(HaxBackend& backend, void*& backendHandle)
{
	MODULEENTRY32W me = {0};
	me.dwSize = sizeof(MODULEENTRY32W);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, GetCurrentProcessId());
	if (Module32FirstW(snapshot, &me))
	{
		do
		{
			if (wcsstr(me.szModule, L"mono") != nullptr)
			{
				backend = HaxBackend_Mono;
				backendHandle = me.hModule;
				CloseHandle(snapshot);
				return true;
			}
			if (wcscmp(me.szModule, L"GameAssembly.dll") == 0)
			{
				backend = HaxBackend_Il2cpp;
				backendHandle = me.hModule;
				CloseHandle(snapshot);
				return true;
			}
		} while (Module32NextW(snapshot, &me));
	}

	CloseHandle(snapshot);
	return false;
}