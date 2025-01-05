#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <format>

#include "haxsdk.h"
#include "third_party/imgui/imgui.h"

void HaxSdk::DoOnceBeforeRendering() {
    HaxSdk::GetGlobals().Load();
    ImGui::GetIO().IniFilename = NULL;
}

void HaxSdk::RenderBackground() {
    
}

void HaxSdk::RenderMenu() {
    HaxGlobals& globals = HaxSdk::GetGlobals();
    ImGui::SetNextWindowSize(ImVec2(globals.m_menuWidth, globals.m_menuHeight), ImGuiCond_Once);
    ImGui::Begin("Window");
    ImGui::Button("123");

    globals.m_menuWidth = ImGui::GetWindowWidth();
    globals.m_menuHeight = ImGui::GetWindowHeight();
    ImGui::End();
}

static void Start() {
    HaxSdk::InitLogger(true);
	HaxSdk::InitializeBackendData();
    HaxSdk::InitializeUnityData();
    HaxSdk::ImplementImGui(GraphicsApi_DirectX11);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}