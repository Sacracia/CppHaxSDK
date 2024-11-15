#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "logger/logger.h"
#include "gui/gui.h"
#include "user.h"
#include "mono/mono.h"
#include "../hooks/hooks.h"

static void Start() {
	LOG_INIT(DEBUG, true);
	mono::Initialize();

    //hooks::SetupHooks();

    ImplementationDetails details;
    details.ApplyStyleProc = ApplyStyle;
    details.DrawMenuProc = RenderMenu;
	haxsdk::ImplementImGui(details);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}