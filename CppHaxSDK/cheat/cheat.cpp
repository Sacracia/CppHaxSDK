#include "cheat.h"

#include <iostream>
#include <assert.h>

#include "../mono/mono_sdk.h"
#include "hooks/hooks.h"
#include "../third_party/imgui/imgui.h"

namespace hero_controller {
    namespace static_fields {
        void* _instance;
    }

    namespace offsets {
        int playerData;
        int transform;
    }

    namespace funcs {
        void(*HeroController_AddGeo)(MonoObject* __this, int amount);
        void(*HeroController_Update)(MonoObject* __this);
    }
}

namespace player_data {
    namespace static_fields {
        void* _instance;
    }

    namespace offsets {
        int infiniteAirJump;
        int isInvincible;
        int scenesVisited;
    }

    namespace funcs {
        int32_t(*PlayerData_getCurrentMaxHealth)(MonoObject* __this);
    }
}

namespace transform {
    namespace funcs {
        MonoObject* (*Transform_getPosition)(MonoObject* __this);
    }
}

namespace cheat_manager {
    namespace funcs {
        bool(*CheatManager_getIsInstaKillEnabled)(MonoObject* __this);
    }
}

#include "game/game_classes.h"

struct ImplementationDetails {
    void(*ApplyStyleProc)();
    void(*DrawMenuProc)(bool*); // bool* added to be compatible with ImGui::ShowDemoWindow
};

extern ImplementationDetails details;
bool isInstaKill;
bool isInvincible;

static ImVec4 HexToColor(std::string hex_string);
static void InitGameData();
static void RenderMenu(bool*);
static void ApplyStyle();

void cheat::Initialize() {
    InitGameData();
    hooks::SetupHooks();

    details.ApplyStyleProc = ApplyStyle;
    details.DrawMenuProc = RenderMenu;
}

static void InitGameData() {
#define MONO_ASSEMBLY   "Assembly-CSharp"
#define MONO_NAMESPACE  ""

#define MONO_CLASS      "HeroController"
    hero_controller::static_fields::_instance = mono::GetStaticFieldAddress(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "_instance");
    hero_controller::offsets::playerData = mono::GetFieldOffset(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "playerData");
    hero_controller::offsets::transform = mono::GetFieldOffset(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "transform");
    hero_controller::funcs::HeroController_AddGeo = (void(*)(MonoObject*, int))mono::GetFuncAddress(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "AddGeo (int)");
    hero_controller::funcs::HeroController_Update = (void(*)(MonoObject*))mono::GetFuncAddress(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "Update ()");
#undef MONO_CLASS
#define MONO_CLASS      "PlayerData"
    player_data::static_fields::_instance = mono::GetStaticFieldAddress(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "_instance");
    player_data::offsets::infiniteAirJump = mono::GetFieldOffset(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "infiniteAirJump");
    player_data::offsets::isInvincible = mono::GetFieldOffset(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "isInvincible");
    player_data::offsets::scenesVisited = mono::GetFieldOffset(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "scenesVisited");
    player_data::funcs::PlayerData_getCurrentMaxHealth = (int32_t(*)(MonoObject*))mono::GetFuncAddress(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "get_CurrentMaxHealth ()");
#undef MONO_CLASS
#define MONO_CLASS      "CheatManager"
    cheat_manager::funcs::CheatManager_getIsInstaKillEnabled = (bool(*)(MonoObject*))mono::GetFuncAddress(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "get_IsInstaKillEnabled ()");
#undef MONO_CLASS

#undef MONO_NAMESPACE
#undef MONO_ASSEMBLY


#define MONO_ASSEMBLY   "UnityEngine.CoreModule"
#define MONO_NAMESPACE  "UnityEngine"
#define MONO_CLASS      "Transform"
    transform::funcs::Transform_getPosition = (MonoObject*(*)(MonoObject*))mono::GetFuncAddress(MONO_ASSEMBLY, MONO_NAMESPACE, MONO_CLASS, "get_position ()");
#undef MONO_ASSEMBLY
#undef MONO_NAMESPACE
#undef MONO_CLASS
}

static void RenderMenu(bool*) {
    HeroController* pHeroController = HeroController::_instance();

    ImGui::SetNextWindowBgAlpha(1);
    ImGui::Begin("Menu", NULL);
    if (pHeroController) {
        PlayerData* pPlayerData = pHeroController->playerData();
        Transform* pTransform = pHeroController->transform();

        ImGui::Checkbox("Infinite Air Jump", &pPlayerData->infiniteAirJump());
        ImGui::Checkbox("InstaKill", &isInstaKill);
        ImGui::Checkbox("Invincible", &isInvincible);
        if (ImGui::Button("+100 Geo")) {
            pHeroController->AddGeo(100);
        }
        ImGui::SameLine();
        if (ImGui::Button("+1000 Geo")) {
            pHeroController->AddGeo(1000);
        }
        ImGui::SameLine();
        if (ImGui::Button("+10000 Geo")) {
            pHeroController->AddGeo(10000);
        }
        if (ImGui::Button("Print scenesVisited")) {
            auto pScenesVisited = pPlayerData->scenesVisited();
            std::cout << pScenesVisited << '\n';
            std::cout << "_items : " << pScenesVisited->_items << ' ' << &pScenesVisited->_items << '\n';
            std::cout << "_size : " << pScenesVisited->_size << ' ' << &pScenesVisited->_size << '\n';
            String** items = pScenesVisited->_items->vector;
            std::cout << items << '\n';

            for (int i = 0; i < pScenesVisited->_size; i++) {
                std::wcout << items[i]->chars << '\n';
            }
        }
        Vector3 pos = pTransform->get_position();
        ImGui::Text("%f %f %f", pos.x, pos.y, pos.z);
    }
    ImGui::End();
}

void ApplyStyle() {
    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\trebucbd.ttf", 13);

    ImGuiStyle* styles = &ImGui::GetStyle();
    auto colors = styles->Colors;
    colors[ImGuiCol_Border] = HexToColor("26383FFF");
    colors[ImGuiCol_BorderShadow] = HexToColor("33333300");
    colors[ImGuiCol_Button] = HexToColor("23303DFF");
    colors[ImGuiCol_ButtonActive] = HexToColor("474968FF");
    colors[ImGuiCol_ButtonHovered] = HexToColor("444C70FF");
    colors[ImGuiCol_CheckMark] = HexToColor("A5BCDBFF");
    colors[ImGuiCol_ChildBg] = HexToColor("1E262BFF");
    colors[ImGuiCol_DragDropTarget] = HexToColor("FFFF00E5");
    colors[ImGuiCol_FrameBg] = HexToColor("2D3F44FF");
    colors[ImGuiCol_FrameBgActive] = HexToColor("30383DFF");
    colors[ImGuiCol_FrameBgHovered] = HexToColor("26303DFF");
    colors[ImGuiCol_Header] = HexToColor("0000003D");
    colors[ImGuiCol_HeaderActive] = HexToColor("0070EAFF");
    colors[ImGuiCol_HeaderHovered] = HexToColor("1E2833CC");
    colors[ImGuiCol_MenuBarBg] = HexToColor("1E232DFF");
    colors[ImGuiCol_ModalWindowDimBg] = HexToColor("CCCCCC59");
    colors[ImGuiCol_NavHighlight] = HexToColor("4296F9FF");
    colors[ImGuiCol_NavWindowingDimBg] = HexToColor("CCCCCC33");
    colors[ImGuiCol_NavWindowingHighlight] = HexToColor("FFFFFFB2");
    colors[ImGuiCol_PlotHistogram] = HexToColor("E5B200FF");
    colors[ImGuiCol_PlotHistogramHovered] = HexToColor("FF9900FF");
    colors[ImGuiCol_PlotLines] = HexToColor("9B9B9BFF");
    colors[ImGuiCol_PlotLinesHovered] = HexToColor("FF6D59FF");
    colors[ImGuiCol_PopupBg] = HexToColor("14161CEF");
    colors[ImGuiCol_ResizeGrip] = HexToColor("A3C9F93F");
    colors[ImGuiCol_ResizeGripActive] = HexToColor("6D8CB2F2");
    colors[ImGuiCol_ResizeGripHovered] = HexToColor("A5BFDDAA");
    colors[ImGuiCol_ScrollbarBg] = HexToColor("1C1C1C63");
    colors[ImGuiCol_ScrollbarGrab] = HexToColor("875E5EFF");
    colors[ImGuiCol_ScrollbarGrabActive] = HexToColor("8E1919FF");
    colors[ImGuiCol_ScrollbarGrabHovered] = HexToColor("7C3A3AFF");
    colors[ImGuiCol_Separator] = HexToColor("333F49FF");
    colors[ImGuiCol_SeparatorActive] = HexToColor("6B91BFFF");
    colors[ImGuiCol_SeparatorHovered] = HexToColor("4F7299C6");
    colors[ImGuiCol_SliderGrab] = HexToColor("5977ADFF");
    colors[ImGuiCol_SliderGrabActive] = HexToColor("ADCCFFFF");
    colors[ImGuiCol_Tab] = HexToColor("1C262BFF");
    colors[ImGuiCol_TabActive] = HexToColor("333F49FF");
    colors[ImGuiCol_TabHovered] = HexToColor("969696CC");
    colors[ImGuiCol_TabUnfocused] = HexToColor("1C262BFF");
    colors[ImGuiCol_TabUnfocusedActive] = HexToColor("1C262BFF");
    colors[ImGuiCol_TableBorderLight] = HexToColor("3A3A3FFF");
    colors[ImGuiCol_TableBorderStrong] = HexToColor("4F4F59FF");
    colors[ImGuiCol_TableHeaderBg] = HexToColor("303033FF");
    colors[ImGuiCol_TableRowBg] = HexToColor("333F49FF");
    colors[ImGuiCol_TableRowBgAlt] = HexToColor("1C262BFF");
    colors[ImGuiCol_Text] = HexToColor("F2F4F9FF");
    colors[ImGuiCol_TextDisabled] = HexToColor("2B353DFF");
    colors[ImGuiCol_TextSelectedBg] = HexToColor("4296F959");
    colors[ImGuiCol_TitleBg] = HexToColor("232D38A5");
    colors[ImGuiCol_TitleBgActive] = HexToColor("212830FF");
    colors[ImGuiCol_TitleBgCollapsed] = HexToColor("26262682");
    colors[ImGuiCol_WindowBg] = HexToColor("1E2623FF");

    //Styles
    styles->Alpha = 1.0;
    styles->AntiAliasedFill = true;
    styles->AntiAliasedLines = true;
    styles->AntiAliasedLinesUseTex = true;
    styles->ButtonTextAlign = ImVec2(0.5, 0.5);
    styles->CellPadding = ImVec2(4.0, 2.0);
    styles->ChildBorderSize = 1.0;
    styles->ChildRounding = 5.0;
    styles->CircleTessellationMaxError = 0.30000001192092896;
    styles->ColorButtonPosition = ImGuiDir_Right;
    styles->ColumnsMinSpacing = 6.0;
    styles->CurveTessellationTol = 1.25;
    styles->DisabledAlpha = 0.6000000238418579;
    styles->DisplaySafeAreaPadding = ImVec2(3.0, 3.0);
    styles->DisplayWindowPadding = ImVec2(19.0, 19.0);
    styles->FrameBorderSize = 0.0;
    styles->FramePadding = ImVec2(4.0, 3.0);
    styles->FrameRounding = 4.0;
    styles->GrabMinSize = 10.0;
    styles->GrabRounding = 4.0;
    styles->IndentSpacing = 21.0;
    styles->ItemInnerSpacing = ImVec2(4.0, 4.0);
    styles->ItemSpacing = ImVec2(8.0, 4.0);
    styles->LogSliderDeadzone = 4.0;
    styles->MouseCursorScale = 1.0;
    styles->PopupBorderSize = 1.0;
    styles->PopupRounding = 0.0;
    styles->ScrollbarRounding = 9.0;
    styles->ScrollbarSize = 14.0;
    styles->SelectableTextAlign = ImVec2(0.0, 0.0);
    styles->TabBorderSize = 0.0;
    styles->TabMinWidthForCloseButton = 0.0;
    styles->TabRounding = 4.0;
    styles->TouchExtraPadding = ImVec2(0.0, 0.0);
    styles->WindowBorderSize = 1.0;
    styles->WindowMenuButtonPosition = ImGuiDir_Left;
    styles->WindowMinSize = ImVec2(32.0, 32.0);
    styles->WindowPadding = ImVec2(8.0, 8.0);
    styles->WindowRounding = 0.0;
    styles->WindowTitleAlign = ImVec2(0.0, 0.5);
}

static ImVec4 HexToColor(std::string hex_string) {
    int r, g, b, a;
    sscanf_s(hex_string.c_str(), "%02x%02x%02x%02x", &r, &g, &b, &a);
    ImVec4 color{ (float(r) / 255), (float(g) / 255), (float(b) / 255), (float(a) / 255) };
    return color;
}