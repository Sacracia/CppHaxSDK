#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <format>

#include "haxsdk_unity.h"
#include "haxsdk_gui.h"

namespace RepoCheat
{
    void Start();
}

static void Start() {
    HaxSdk::Initialize(true);
    RepoCheat::Start();

    HaxSdk::ImplementImGui(GraphicsApi_DirectX11);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().m_CheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}