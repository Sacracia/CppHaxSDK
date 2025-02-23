#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk.h"
#include "logger/Logger.h"
#include "haxsdk_gui.h"

void            HaxSdk::AttachMenuToUnityThread() {

}
void            HaxSdk::DoOnceBeforeRendering() {

}
void            HaxSdk::RenderMenu() {

}
void            HaxSdk::RenderBackground() {

}

static void Start() {
	HaxSdk::InitLogger(true);
	HaxSdk::InitializeCore();
	std::cout << std::hex << HaxSdk::GetGlobals().backendHandle << '\n' << HaxSdk::GetGlobals().cheatModule;
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().cheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}