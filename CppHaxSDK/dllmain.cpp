#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "logger/logger.h"
#define HAX_API extern
#include "mono/mono_api.h"

#include "imgui.h"

void HaxSdk::ApplyStyle() {

}

void HaxSdk::RenderBackground() {

}

void HaxSdk::RenderMenu() {
    ImGui::ShowDemoWindow();
}

static void Start() {
	LOG_INIT(DEBUG, true);
	HaxSdk::InitializeMono();
    MonoClass* pKlass = HaxSdk::GetUnityClass("Assembly-CSharp", "", "AI_NetworkBehaviour_Animal");
    Array<MonoObject*>* animals = Object::FindObjectsOfType(mono_class_get_type(pKlass));
    for (int i = 0; i < animals->max_length; ++i) {
        std::cout << "Animal: " << animals->vector[i] << '\n';
    }
	//HaxSdk::ImplementImGui(GraphicsApi_DirectX11 | GraphicsApi_DirectX12);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}