#include "cheat.h"

#include <iostream>
#include <assert.h>

#include "../third_party/imgui/imgui.h"
#include "../gui/gui.h"
#include "hooks/hooks.h"

#define MONO_API_FUNC(r, n, p)          extern r(*n)p
#define MONO_STATIC_FIELD(n, r, c)      extern r c ## _ ## n
#define MONO_FIELD_OFFSET(n, c)         extern int c ## _ ## n
#define MONO_GAME_FUNC(r, n, p, c, s)   extern r(__fastcall *c ## _ ## n)p
#include "../mono/mono_api_classes.h"
#include "../mono/mono_api_functions.h"
#include "../mono/mono_game_data.h"

class Spell1 {
public:
    int backField;

    int Get() {
        PlayerData* pPlayerData = PlayerData::_instance();
        return pPlayerData->hasSpell() ? pPlayerData->fireballLevel() : 0;
    }

    void Set(int value) {
        PlayerData* pPlayerData = PlayerData::_instance();
        if (value > 0)
            pPlayerData->hasSpell() = true;
        pPlayerData->fireballLevel() = value;
    }
};

class Spell2 {
public:
    int Get() {
        PlayerData* pPlayerData = PlayerData::_instance();
        return pPlayerData->hasSpell() ? pPlayerData->quakeLevel() : 0;
    }

    void Set(int value) {
        PlayerData* pPlayerData = PlayerData::_instance();
        if (value > 0)
            pPlayerData->hasSpell() = true;
        pPlayerData->quakeLevel() = value;
    }
};

class Spell3 {
public:
    int Get() {
        PlayerData* pPlayerData = PlayerData::_instance();
        return pPlayerData->hasSpell() ? pPlayerData->screamLevel() : 0;
    }

    void Set(int value) {
        PlayerData* pPlayerData = PlayerData::_instance();
        if (value > 0)
            pPlayerData->hasSpell() = true;
        pPlayerData->screamLevel() = value;
    }
};

extern ImplementationDetails details;
bool isInstaKill;
bool isInvincible;
bool isFastAttack;
bool isInfiniteDash;
bool isInfiniteSoul;
float speedMultiplier = 1.;
int geoMultiplier = 1;

static void RenderMenu(bool*);
static void RenderHeroTab();
static void RenderMapTab();
static void RenderPowerUpsTab();
static ImVec4 HexToColor(std::string hex_string);
static void ApplyStyle();

void cheat::Initialize() {
    hooks::SetupHooks();

    details.api = Dx11;
    details.ApplyStyleProc = ApplyStyle;
    details.DrawMenuProc = RenderMenu;
}

static void RenderMenu(bool*) {
    ImGui::SetNextWindowBgAlpha(1);
    ImGui::Begin("Menu", NULL);
    ImGui::BeginTabBar("#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons);
    RenderHeroTab();
    RenderMapTab();
    RenderPowerUpsTab();
    ImGui::EndTabBar();
    /*if (pHeroController) {
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
    }*/
    ImGui::End();
}

static void RenderHeroTab() {
    if (ImGui::BeginTabItem("Hero")) {
        HeroController* pHeroController = HeroController::_instance();
        if (pHeroController) {
            PlayerData* pPlayerData = pHeroController->playerData();
            ImGui::SeparatorText("HEALTH");
            ImGui::Checkbox("Invincible", &isInvincible);
            if (ImGui::Button("Restore health")) {
                pHeroController->MaxHealth();
            }

            ImGui::SeparatorText("DAMAGE");
            ImGui::Checkbox("Insta kill", &isInstaKill);
            ImGui::Checkbox("Fast attacks", &isFastAttack);

            ImGui::SeparatorText("MOVEMENT");
            ImGui::SliderFloat("Speed multiplier", &speedMultiplier, 1., 5., "%.1f");
            ImGui::Checkbox("Infinite jumps", &pPlayerData->infiniteAirJump());
            ImGui::Checkbox("Infinite dash", &isInfiniteDash);
            // has dash skipped
            ImGui::SliderFloat("Dash distance", &pHeroController->DASH_SPEED(), 0., 80., "%.1f");

            ImGui::SeparatorText("GEO");
            if (ImGui::Button("+100 geo"))
                pHeroController->AddGeo(100);
            ImGui::SameLine();
            if (ImGui::Button("+1000 geo"))
                pHeroController->AddGeo(1000);
            ImGui::SameLine();
            if (ImGui::Button("+10000 geo"))
                pHeroController->AddGeo(10000);
            ImGui::SliderInt("Geo multiplier", &geoMultiplier, 1, 10);

            ImGui::SeparatorText("MISC");
            ImGui::Checkbox("Infinite soul", &isInfiniteSoul);
            float timeScale = game::funcs::Time_get_timeScale(nullptr);
            float timeScaleCopy = timeScale;
            ImGui::SliderFloat("Time scale", &timeScale, 1., 4., "%.1f");
            if (timeScale != timeScaleCopy)
                game::funcs::Time_set_timeScale(timeScale);
        }
        ImGui::EndTabItem();
    }
}

static void RenderMapTab() {
    if (ImGui::BeginTabItem("Map")) {
        HeroController* pHeroController = HeroController::_instance();
        if (pHeroController) {
            PlayerData* pPlayerData = pHeroController->playerData();
            ImGui::SeparatorText("UNLOCKABLE");
            ImGui::Checkbox("Has map", &pPlayerData->hasMap());
            ImGui::Checkbox("Mapper shop", &pPlayerData->openedMapperShop());
            ImGui::Checkbox("Has quill", &pPlayerData->hasQuill());
            ImGui::Checkbox("Has compass", &pPlayerData->gotCharm_2());

            ImGui::SeparatorText("REGIONS");
            ImGui::Checkbox("Dirtmouth", &pPlayerData->mapDirtmouth());
            ImGui::Checkbox("Crossroads", &pPlayerData->mapCrossroads());
            ImGui::Checkbox("Greenpath", &pPlayerData->mapGreenpath());
            ImGui::Checkbox("FogCanyon", &pPlayerData->mapFogCanyon());
            ImGui::Checkbox("Royal Gardens", &pPlayerData->mapRoyalGardens());
            ImGui::Checkbox("Fungal Wastes", &pPlayerData->mapFungalWastes());
            ImGui::Checkbox("City", &pPlayerData->mapCity());
            ImGui::Checkbox("Waterways", &pPlayerData->mapWaterways());
            ImGui::Checkbox("Mines", &pPlayerData->mapMines());
            ImGui::Checkbox("Deepnest", &pPlayerData->mapDeepnest());
            ImGui::Checkbox("Cliffs", &pPlayerData->mapCliffs());
            ImGui::Checkbox("Outskirts", &pPlayerData->mapOutskirts());
            ImGui::Checkbox("Resting Grounds", &pPlayerData->mapRestingGrounds());
            ImGui::Checkbox("Abyss", &pPlayerData->mapAbyss());

            ImGui::SeparatorText("PINS");
            ImGui::Checkbox("Has pin", &pPlayerData->hasPin());
            ImGui::Checkbox("Bench", &pPlayerData->hasPinBench());
            ImGui::Checkbox("Cocoon", &pPlayerData->hasPinCocoon());
            ImGui::Checkbox("Dream plant", &pPlayerData->hasPinDreamPlant());
            ImGui::Checkbox("Guardian", &pPlayerData->hasPinGuardian());
            ImGui::Checkbox("Black egg", &pPlayerData->hasPinBlackEgg());
            ImGui::Checkbox("Shop", &pPlayerData->hasPinShop());
            ImGui::Checkbox("Spa", &pPlayerData->hasPinSpa());
            ImGui::Checkbox("Stag", &pPlayerData->hasPinStag());
            ImGui::Checkbox("Tram", &pPlayerData->hasPinTram());
            ImGui::Checkbox("Ghost", &pPlayerData->hasPinGhost());
            ImGui::Checkbox("Grub", &pPlayerData->hasPinGrub());
        }
        ImGui::EndTabItem();
    }
}

static void RenderPowerUpsTab() {
    static const char* spell3Levels[3] = { "Locked", "Howling Wraiths", "Abyss Shriek" };

    if (ImGui::BeginTabItem("Powerups")) {
        HeroController* pHeroController = HeroController::_instance();
        if (pHeroController) {
            static const char* spell1Levels[3] = { "Locked", "Vengeful Spirit", "Shade Soul" };
            Spell1 spell1;
            int selected = spell1.Get();
            if (ImGui::BeginCombo("Spell1", spell1Levels[selected]))
            {
                for (int n = 0; n < 3; n++)
                {
                    const int selected = spell1.Get();
                    const bool isSelected = (selected == n);
                    if (ImGui::Selectable(spell1Levels[n], isSelected))
                        spell1.Set(n);

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            static const char* spell2Levels[3] = { "Locked", "Desolate Dive", "Descending Dark" };
            Spell2 spell2;
            selected = spell2.Get();
            if (ImGui::BeginCombo("Spell2", spell2Levels[selected]))
            {
                for (int n = 0; n < 3; n++)
                {
                    const int selected = spell2.Get();
                    const bool isSelected = (selected == n);
                    if (ImGui::Selectable(spell2Levels[n], isSelected))
                        spell2.Set(n);

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            static const char* spell3Levels[3] = { "Locked", "Howling Wraiths", "Abyss Shriek" };
            Spell3 spell3;
            selected = spell3.Get();
            if (ImGui::BeginCombo("Spell3", spell3Levels[selected]))
            {
                for (int n = 0; n < 3; n++)
                {
                    const int selected = spell3.Get();
                    const bool isSelected = (selected == n);
                    if (ImGui::Selectable(spell3Levels[n], isSelected))
                        spell3.Set(n);

                    if (isSelected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

        }
        ImGui::EndTabItem();
    }
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