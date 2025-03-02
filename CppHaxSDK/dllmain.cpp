#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk.h"
#include "haxsdk_gui.h"
#include "unity/haxsdk_unity.h"

#include "third_party/imgui/imgui.h"

void HaxSdk::DoOnceBeforeRendering() {

}

void HaxSdk::RenderMenu() {
    static bool flag = true;
    if (flag) {
        flag = false;
        HaxSdk::Log("Menu must be rendered\n");
    }
    ImGui::ShowDemoWindow();
}

void HaxSdk::RenderBackground() {
    static bool flag = true;
    if (flag) {
        flag = false;
        HaxSdk::Log("Background must be rendered\n");
    }
}

static void Start() {
	HaxSdk::InitLogger(false);
	HaxSdk::InitializeCore();
    HaxSdk::ImplementImGui(GraphicsApi_Any);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	HaxSdk::GetGlobals().cheatModule = (void*)(module);
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}