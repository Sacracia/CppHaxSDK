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

class HasDash {
public:
    bool Get() {
        PlayerData* pPlayerData = PlayerData::_instance();
        return pPlayerData->hasDash() && pPlayerData->canDash();
    }

    void Set(bool value) {
        PlayerData* pPlayerData = PlayerData::_instance();
        pPlayerData->hasDash() = pPlayerData->canDash() = value;
    }
};

class HasShadowDash {
public:
    bool Get() {
        PlayerData* pPlayerData = PlayerData::_instance();
        return pPlayerData->hasDash() && pPlayerData->canDash() && pPlayerData->hasShadowDash() && pPlayerData->canShadowDash();
    }

    void Set(bool value) {
        PlayerData* pPlayerData = PlayerData::_instance();
        pPlayerData->hasShadowDash() = pPlayerData->canShadowDash() = value;
        if (value)
            pPlayerData->hasDash() = pPlayerData->canDash() = true;
    }
};

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

    details.api = Dx11 | Dx12;
    details.ApplyStyleProc = ApplyStyle;
    details.DrawMenuProc = RenderMenu;
}

static void RenderMenu(bool*) {
    ImGui::SetNextWindowSize(ImVec2(400, 550), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(1);
    ImGui::Begin("Hollow Knight Cheat Menu [by Sacracia]", NULL);
    ImGui::BeginTabBar("#TopBar", ImGuiTabBarFlags_NoTabListScrollingButtons);
    RenderHeroTab();
    RenderMapTab();
    RenderPowerUpsTab();
    ImGui::EndTabBar();
    ImGui::End();
}

static void RenderHeroTab() {
    if (ImGui::BeginTabItem("Hero")) {
        HeroController* pHeroController = HeroController::_instance();
        if (pHeroController) {
            PlayerData* pPlayerData = pHeroController->playerData();
            ImGui::SeparatorText("HEALTH");
            ImGui::Checkbox("Invincible", &isInvincible);
            if (ImGui::Button("Restore health"))
                pHeroController->AddHealth(pPlayerData->maxHealth() - pPlayerData->health());
            ImGui::Spacing();

            ImGui::SeparatorText("DAMAGE");
            ImGui::Checkbox("Insta kill", &isInstaKill);
            ImGui::Checkbox("Fast attacks", &isFastAttack);
            ImGui::Spacing();

            ImGui::SeparatorText("MOVEMENT");
            ImGui::SliderFloat("Speed multiplier", &speedMultiplier, 1., 5., "%.1f");
            ImGui::Checkbox("Infinite jumps", &pPlayerData->infiniteAirJump());
            ImGui::Checkbox("Infinite dash", &isInfiniteDash);
            HasDash hasDash;
            bool dashUnlocked = hasDash.Get();
            bool dashUnlockedCopy = dashUnlocked;
            ImGui::Checkbox("Dash unlocked", &dashUnlocked);
            if (dashUnlocked != dashUnlockedCopy)
                hasDash.Set(dashUnlocked);
            HasShadowDash hasShadowDash;
            dashUnlocked = hasShadowDash.Get();
            dashUnlockedCopy = dashUnlocked;
            ImGui::Checkbox("Shadow dash unlocked", &dashUnlocked);
            if (dashUnlocked != dashUnlockedCopy)
                hasShadowDash.Set(dashUnlocked);
            ImGui::SliderFloat("Dash distance", &pHeroController->DASH_SPEED(), 0., 80., "%.1f");
            ImGui::Spacing();

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
            ImGui::Spacing();

            ImGui::SeparatorText("MISC");
            ImGui::Checkbox("Infinite soul", &isInfiniteSoul);
            float timeScale = game::funcs::Time_get_timeScale(nullptr);
            float timeScaleCopy = timeScale;
            ImGui::SliderFloat("Time scale", &timeScale, 1., 4., "%.1f");
            if (timeScale != timeScaleCopy)
                game::funcs::Time_set_timeScale(timeScale);
            ImGui::Spacing();
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
            ImGui::Spacing();

            ImGui::SeparatorText("REGIONS");
            if (ImGui::BeginTable("regions", 3))
            {
                ImGui::TableNextColumn(); ImGui::Checkbox("Dirtmouth", &pPlayerData->mapDirtmouth());
                ImGui::TableNextColumn(); ImGui::Checkbox("Crossroads", &pPlayerData->mapCrossroads());
                ImGui::TableNextColumn(); ImGui::Checkbox("Greenpath", &pPlayerData->mapGreenpath());
                ImGui::TableNextColumn(); ImGui::Checkbox("FogCanyon", &pPlayerData->mapFogCanyon());
                ImGui::TableNextColumn(); ImGui::Checkbox("Royal Gardens", &pPlayerData->mapRoyalGardens());
                ImGui::TableNextColumn(); ImGui::Checkbox("Fungal Wastes", &pPlayerData->mapFungalWastes());
                ImGui::TableNextColumn(); ImGui::Checkbox("City", &pPlayerData->mapCity());
                ImGui::TableNextColumn(); ImGui::Checkbox("Waterways", &pPlayerData->mapWaterways());
                ImGui::TableNextColumn(); ImGui::Checkbox("Mines", &pPlayerData->mapMines());
                ImGui::TableNextColumn(); ImGui::Checkbox("Deepnest", &pPlayerData->mapDeepnest());
                ImGui::TableNextColumn(); ImGui::Checkbox("Cliffs", &pPlayerData->mapCliffs());
                ImGui::TableNextColumn(); ImGui::Checkbox("Outskirts", &pPlayerData->mapOutskirts());
                ImGui::TableNextColumn(); ImGui::Checkbox("Resting Grounds", &pPlayerData->mapRestingGrounds());
                ImGui::TableNextColumn(); ImGui::Checkbox("Abyss", &pPlayerData->mapAbyss());
                ImGui::EndTable();
            }
            ImGui::Spacing();

            ImGui::SeparatorText("PINS");
            if (ImGui::BeginTable("pins", 2)) {
                ImGui::TableNextColumn(); ImGui::Checkbox("Has pin", &pPlayerData->hasPin());
                ImGui::TableNextColumn(); ImGui::Checkbox("Bench", &pPlayerData->hasPinBench());
                ImGui::TableNextColumn(); ImGui::Checkbox("Cocoon", &pPlayerData->hasPinCocoon());
                ImGui::TableNextColumn(); ImGui::Checkbox("Dream plant", &pPlayerData->hasPinDreamPlant());
                ImGui::TableNextColumn(); ImGui::Checkbox("Guardian", &pPlayerData->hasPinGuardian());
                ImGui::TableNextColumn(); ImGui::Checkbox("Black egg", &pPlayerData->hasPinBlackEgg());
                ImGui::TableNextColumn(); ImGui::Checkbox("Shop", &pPlayerData->hasPinShop());
                ImGui::TableNextColumn(); ImGui::Checkbox("Spa", &pPlayerData->hasPinSpa());
                ImGui::TableNextColumn(); ImGui::Checkbox("Stag", &pPlayerData->hasPinStag());
                ImGui::TableNextColumn(); ImGui::Checkbox("Tram", &pPlayerData->hasPinTram());
                ImGui::TableNextColumn(); ImGui::Checkbox("Ghost", &pPlayerData->hasPinGhost());
                ImGui::TableNextColumn(); ImGui::Checkbox("Grub", &pPlayerData->hasPinGrub());
                ImGui::EndTable();
            }
            ImGui::Spacing();
        }
        ImGui::EndTabItem();
    }
}

static void RenderPowerUpsTab() {
    static const char* spell3Levels[3] = { "Locked", "Howling Wraiths", "Abyss Shriek" };

    if (ImGui::BeginTabItem("Powerups")) {
        HeroController* pHeroController = HeroController::_instance();
        if (pHeroController) {
            PlayerData* pPlayerData = pHeroController->playerData();
            ImGui::SeparatorText("SPELLS");
            static const char* spell1Levels[3] = { "Locked", "Vengeful Spirit", "Shade Soul" };
            static const char* spell2Levels[3] = { "Locked", "Desolate Dive", "Descending Dark" };
            static const char* spell3Levels[3] = { "Locked", "Howling Wraiths", "Abyss Shriek" };
            Spell1 spell1;
            Spell2 spell2;
            Spell3 spell3;
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
            ImGui::Spacing();

            ImGui::SeparatorText("ABILITIES");
            ImGui::Checkbox("Isma\'s tear", &pPlayerData->hasAcidArmour());
            ImGui::Checkbox("Dream nail", &pPlayerData->hasDreamNail());
            if (ImGui::Button("+1 dream orb"))
                pPlayerData->dreamOrbs() += 1;
            ImGui::SameLine();
            if (ImGui::Button("+10 dream orb"))
                pPlayerData->dreamOrbs() += 10;
            ImGui::SameLine();
            if (ImGui::Button("+100 dream orb"))
                pPlayerData->dreamOrbs() += 100;
            ImGui::Checkbox("Awoken dream nail", &pPlayerData->dreamNailUpgraded());
            ImGui::Checkbox("Dreamgate", &pPlayerData->hasDreamGate());
            ImGui::Spacing();

            ImGui::SeparatorText("KEYS");
            if (ImGui::BeginTable("KEYS", 2)) {
                ImGui::TableNextColumn(); ImGui::Checkbox("City crest", &pPlayerData->hasCityKey());
                ImGui::TableNextColumn(); ImGui::Checkbox("Elegant key", &pPlayerData->hasWhiteKey());
                ImGui::TableNextColumn(); ImGui::Checkbox("Kings brand", &pPlayerData->hasKingsBrand());
                ImGui::TableNextColumn(); ImGui::Checkbox("Love key", &pPlayerData->hasLoveKey());
                ImGui::TableNextColumn(); ImGui::Checkbox("Shopkeeper\'s Key", &pPlayerData->hasSlykey());
                ImGui::EndTable();
            }
            if (ImGui::Button("Add simple key"))
                ++pPlayerData->simpleKeys();
            ImGui::Spacing();

            ImGui::SeparatorText("EXPLORATION & QUESTS");
            if (ImGui::BeginTable("EXPLORATION & QUESTS", 2)) {
                ImGui::TableNextColumn(); ImGui::Checkbox("Tram pass", &pPlayerData->hasTramPass());
                ImGui::TableNextColumn(); ImGui::Checkbox("Lumafly lantern", &pPlayerData->hasLantern());
                ImGui::TableNextColumn(); ImGui::Checkbox("Hunter\'s journal", &pPlayerData->hasJournal());
                ImGui::TableNextColumn(); ImGui::Checkbox("Hunter\'s mask", &pPlayerData->hasHuntersMark());
                ImGui::TableNextColumn(); ImGui::Checkbox("Delicate flower", &pPlayerData->hasXunFlower());
                ImGui::TableNextColumn(); ImGui::Checkbox("Godtuner", &pPlayerData->hasGodfinder());
                ImGui::EndTable();
            }
            if (ImGui::Button("Get all charms")) {
                pPlayerData->hasCharm() = true;
                bool* pCharm = &pPlayerData->gotCharm_1();
                *pCharm = true;
                for (pCharm += 7; pCharm <= &pPlayerData->gotCharm_40(); pCharm += 8)
                    *pCharm = true;
            }
        }
        ImGui::EndTabItem();
    }
}

void ApplyStyle() {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    float fontSize = std::round(13 * (float)desktop.bottom / 1080);
    LOG_DEBUG << "Resolution, font: " << desktop.right << 'x' << desktop.bottom << ' ' << fontSize << LOG_FLUSH;

    ImGuiIO& io = ImGui::GetIO();
    ImFont* font = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\trebucbd.ttf", fontSize);

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