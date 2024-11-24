#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "logger/logger.h"
#include "gui/gui.h"
#include "mono/mono.h"
#include "cheat/cheat.h"
#include "cheat/hooks/hooks.h"

ImplementationDetails details;

static void Start() {
	LOG_INIT(DEBUG, false);
	mono::Initialize();
    cheat::Initialize();
	haxsdk::ImplementImGui(details);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}