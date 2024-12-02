#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "imgui.h"
#include "logger/logger.h"

#include "mono/core.h"
#include "cheat/classes.h"
#include "cheat/globals.h"

static AI_NetworkBehaviour_Animal* pShark = nullptr;
static PersonController* pPlayer = nullptr;

void HaxSdk::ApplyStyle() {

}

void HaxSdk::RenderBackground() {
    if (pShark) {
        Vector3 position = pShark->get_transform()->get_position();
        Vector3 screen = Camera::main()->WorldToScreenPoint(position);
        if (screen.z > 0) {
            Vector3 playerPos = pPlayer->get_transform()->get_position();
            float dist = Vector3::Distance(playerPos, position);
            char buff[10]{};
            std::to_chars(buff, buff + 10, dist);
            ImGui::GetBackgroundDrawList()->AddText(ImVec2(screen.x, globals::g_screenHeight - screen.y), 0xFF0000ff, buff);
        }
    }
}

void HaxSdk::RenderMenu() {
    ImGui::ShowDemoWindow();
}

static void Start() {
	LOG_INIT(DEBUG, true);
	HaxSdk::InitializeMono();
    Type* pType = MonoClass::find("Assembly-CSharp", "", "AI_NetworkBehaviour_Animal")->system_type();
    pShark = (AI_NetworkBehaviour_Animal*)Object::FindObjectsOfType(pType)->at(0);
    pType = MonoClass::find("Assembly-CSharp", "", "PersonController")->system_type();
    pPlayer = (PersonController*)Object::FindObjectOfType(pType);
    std::cout << "Player: " << pPlayer << '\n';

	HaxSdk::ImplementImGui(GraphicsApi_DirectX11 | GraphicsApi_DirectX12);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}