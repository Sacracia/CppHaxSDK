﻿#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "haxsdk.h"
#include "third_party/imgui/imgui.h"

void HaxSdk::ApplyStyle() {

}

void HaxSdk::RenderBackground() {
    
}

void HaxSdk::RenderMenu() {
    ImGui::ShowDemoWindow();
}

static void Start() {
	LOG_INIT(DEBUG, true);
	HaxSdk::InitializeBackend();
    HaxSdk::InitializeUnity();
	HaxSdk::ImplementImGui(GraphicsApi_DirectX11 | GraphicsApi_DirectX12);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}