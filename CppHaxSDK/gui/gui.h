#pragma once

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

namespace HaxSdk {
    void RenderMenu();
    void RenderBackground();
    void ApplyStyle();
    void AttachToThread();
	void ImplementImGui(GraphicsApi);
}