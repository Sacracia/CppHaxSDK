#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef int GraphicsApi;

enum GraphicsApi_ {
    GraphicsApi_None      = 0,
    GraphicsApi_OpenGL    = 1 << 0,
    GraphicsApi_DirectX9  = 1 << 1,
    GraphicsApi_DirectX10 = 1 << 2,
    GraphicsApi_DirectX11 = 1 << 3,
    GraphicsApi_DirectX12 = 1 << 4,
    GraphicsApi_Vulkan    = 1 << 5,
    GraphicsApi_All       = (1 << 6) - 1
};

struct HaxGlobals {
    ~HaxGlobals();
public:
    void    Save();
    void    Load();
public:
    float   m_menuWidth = 500.f;
    float   m_menuHeight = 600.f;
    float   m_screenWidth = -1;
    float   m_screenHeight = -1;
    bool    m_visible = true;
    int     m_language = 0;
    int     m_key = 0xC0;
};

namespace HaxSdk {
    HaxGlobals& GetGlobals();
    void AttachMenuToUnityThread();
    void DoOnceBeforeRendering();
    void RenderMenu();
    void RenderBackground();
    void* LoadTextureFromData(unsigned char* image_data, int image_width, int image_height);
    void ImplementImGui(GraphicsApi);
}