#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk.h"

void HaxSdk::ApplyStyle() {

}

void HaxSdk::RenderBackground() {

}

void HaxSdk::RenderMenu() {

}

static void Start() {
	LOG_INIT(DEBUG, true);
	HaxSdk::InitializeBackendData();
    HaxSdk::InitializeUnityData();

    Class* pClass = Class::Find("VampireSurvivors.Runtime", "VampireSurvivors.Framework", "GameManager");
    std::cout << pClass->FindField("_signalBus")->Offset();
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}