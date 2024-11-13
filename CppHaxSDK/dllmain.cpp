#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>

#include "logger/logger.h"
#include "gui/gui.h"
#include "user.h"
#include "mono/mono.h"

#define MONO_GAME_FUNC(r, n, p, s) extern r(*n)p
#include "mono/mono_game_functions.h"
#undef MONO_GAME_FUNC

#define MONO_API_FUNC(r, n, p) extern r(*n)p
#include "mono/mono_api_functions.h"
#undef MONO_API_FUNC

#include "imgui.h"

static void DrawMenu(bool*) {
    static bool attached = false;
    if (!attached) {
        attached = true;
        MonoDomain* domain = mono_get_root_domain();
        mono_thread_attach(domain);
        mono_thread_attach(mono_domain_get());
    }
    HeroController* pHeroController = *HeroController::_instance();

    ImGui::SetNextWindowBgAlpha(1);
    ImGui::Begin("Menu", NULL);
    if (pHeroController) {
        PlayerData* pPlayerData = *pHeroController->playerData();
        ImGui::Checkbox("Infinite Air Jump", pPlayerData->infiniteAirJump());
        if (ImGui::Button("+100 Geo")) {
            HeroController_AddGeo(pHeroController, 100);
        }
        if (ImGui::Button("+1000 Geo")) {
            HeroController_AddGeo(pHeroController, 1000);
        }
        if (ImGui::Button("+10000 Geo")) {
            HeroController_AddGeo(pHeroController, 10000);
        }
    }
    ImGui::End();
}

static void Start() {
	LOG_INIT(DEBUG, true);
	mono::Initialize();

    ImplementationDetails details;
    details.ApplyStyleProc = ApplyStyle;
    details.DrawMenuProc = DrawMenu;
	haxsdk::ImplementImGui(details);

    HeroController* pHeroController = *HeroController::_instance();
    HeroController_AddGeo(pHeroController, 1000);
}

bool __stdcall DllMain(HMODULE module, DWORD reason, LPVOID lpvReserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Start, NULL, 0, NULL);
	}
	return true;
}