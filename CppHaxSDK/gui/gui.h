#pragma once

struct ImplementationDetails {
    void(*ApplyStyleProc)();
    void(*DrawMenuProc)(bool*); // bool* added to be compatible with ImGui::ShowDemoWindow
};

namespace haxsdk {
	void ImplementImGui(ImplementationDetails& details);
}