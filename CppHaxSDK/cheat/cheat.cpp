#include "cheat.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <cmath>

#include "../third_party/imgui/imgui.h"
#include "../gui/gui.h"
#include "hooks/hooks.h"
#include "../logger/logger.h"

#define MONO_API_FUNC(r, n, p)          extern r(*n)p
#define MONO_STATIC_FIELD(n, r, c)      extern r c ## _ ## n
#define MONO_FIELD_OFFSET(n, c)         extern int c ## _ ## n
#define MONO_GAME_FUNC(r, n, p, c, s)   extern r(__fastcall *c ## _ ## n)p
#include "../mono/mono_api_classes.h"
#include "../mono/mono_api_functions.h"
#include "../mono/mono_game_data.h"

long windowWidth, windowHeight;

static void RenderMenu(bool*);

void cheat::Initialize(ImplementationDetails& details) {
    //hooks::SetupHooks();

    details.api = Dx11 | Dx12;
    details.DrawMenuProc = RenderMenu;
}

static void RenderMenu(bool*) {
    ImGui::SetNextWindowSize(ImVec2(400, 550), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(1);
    ImGui::Begin("Menu", NULL);
    
    HeroController* pHeroController = HeroController::_instance();
    if (pHeroController) {
        Camera* pMainCamera = GameCameras::_instance()->mainCamera();
        Vector3 playerPos = pHeroController->transform()->get_position();
        Vector3 foot = pMainCamera->WorldToScreenPoint(playerPos);

        ImGui::GetBackgroundDrawList()->AddLine(ImVec2(windowWidth / 2.F, windowHeight), ImVec2(foot.x, windowHeight - foot.y), 0xFF0000ff, 1.5F);
    }

    ImGui::End();
}