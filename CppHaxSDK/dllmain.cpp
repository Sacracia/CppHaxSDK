#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk.h"
#include "haxsdk_gui.h"
#include "haxsdk_unity.h"

#include "third_party/imgui/imgui.h"
#include <thread>

void HaxSdk::DoOnceBeforeRendering() {

}

void HaxSdk::RenderMenu() {
    ImGui::ShowDemoWindow();
}

void HaxSdk::RenderBackground() {
    
}

static void Start() {
	HaxSdk::InitLogger(true);
	HaxSdk::InitializeCore();
    HaxSdk::ImplementImGui(GraphicsApi_DirectX11);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().cheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}