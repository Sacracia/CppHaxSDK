#pragma once

enum ImplementationApi {
    All  = 0,
    Dx11 = 1 << 0,
    Dx12 = 1 << 1
};

struct ImplementationDetails {
    int api = All;
    void(*ApplyStyleProc)();
    void(*DrawMenuProc)(bool*); // bool* added to be compatible with ImGui::ShowDemoWindow
};

namespace haxsdk {
	void ImplementImGui(ImplementationDetails& details);
}