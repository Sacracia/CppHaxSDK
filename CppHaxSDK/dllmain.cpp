#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

static void Start() {

}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().cheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}