#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk.h"
#include "cheat/cheat.h"

static void Start() {
	LOG_INIT(DEBUG, true);
	HaxSdk::InitializeBackendData();
    HaxSdk::InitializeUnityData();
    HaxSdk::InitializeGameData();
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}