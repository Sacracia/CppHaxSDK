#pragma once

#include <cstdint>
#include <functional>

#include "third_party/imgui/imgui.h"

using HANDLE = void*;

typedef int GraphicsApi;

enum GraphicsApi_ 
{
    GraphicsApi_None      = 0,
    GraphicsApi_OpenGL    = 1 << 0,
    GraphicsApi_DirectX9  = 1 << 1,
    GraphicsApi_DirectX10 = 1 << 2,
    GraphicsApi_DirectX11 = 1 << 3,
    GraphicsApi_DirectX12 = 1 << 4,
    GraphicsApi_Vulkan    = 1 << 5,
    GraphicsApi_Any       = (1 << 6) - 1
};

enum TextShift {
    TextShift_Right = 0,
    TextShift_Left = 1,
    TextShift_Center = 2
};

struct HaxTexture 
{
    void*           m_pTexture;
    float           m_width;
    float           m_height;
};

struct HaxWindow
{
    HaxWindow(const char* name, bool* open = nullptr, ImGuiWindowFlags flags = 0)
    {
        m_Flag = ImGui::Begin(name, open, flags);
    }

    ~HaxWindow()
    {
        if (m_Flag)
            ImGui::End();
    }

    explicit operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

struct HaxTable
{
    HaxTable(const char* str_id, int columns, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f)
    {
        m_Flag = ImGui::BeginTable(str_id, columns, flags, outer_size, inner_width);
    }

    ~HaxTable()
    {
        if (m_Flag)
            ImGui::EndTable();
    }

    inline operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

struct HaxCombo
{
    HaxCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0)
    {
        m_Flag = ImGui::BeginCombo(label, preview_value, flags);
    }

    ~HaxCombo()
    {
        if (m_Flag)
            ImGui::EndCombo();
    }

    inline operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

struct HaxDisabled
{
    HaxDisabled(bool disabled = true)
    {
        ImGui::BeginDisabled(disabled);
    }

    ~HaxDisabled()
    {
        ImGui::EndDisabled();
    }
};

struct HaxTooltip
{
    HaxTooltip()
    {
        m_Flag = ImGui::BeginTooltip();
    }

    ~HaxTooltip()
    {
        if (m_Flag)
            ImGui::EndTooltip();
    }

    inline operator bool() const { return m_Flag; }

private:
    bool m_Flag;
};

namespace HaxSdk {
    void            Shutdown();
    HaxTexture      LoadTextureFromResource(int32_t id);
    void            ImplementImGui(GraphicsApi graphics, std::function<void()> fnInit, std::function<void()> fnBackground, std::function<void()> fnRender);
    void            AddText(ImFont* font, const char* text, const ImVec2& pos, ImU32 col, float fontSize, TextShift shift);

    void            SetCheatHandle(HANDLE handle);
}